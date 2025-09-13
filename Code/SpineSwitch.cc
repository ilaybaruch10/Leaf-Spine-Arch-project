
#include "SpineSwitch.h"
#include <algorithm> // Include for std::string

Define_Module(SpineSwitch);

SpineSwitch::SpineSwitch()
{
    messagesForwarded = 0;
    messagesFlooded = 0;
    macAddressesLearned = 0;
    totalMessages = 0;
    messagesDroppedQueueFull = 0; // Initialize new counter
    isProcessing = false;
    processingCompleteMsg = nullptr;
}

SpineSwitch::~SpineSwitch()
{
    cancelAndDelete(processingCompleteMsg);
}

void SpineSwitch::initialize()
{
    // Get parameters
    processingDelay = par("processingDelay").doubleValue();
    queueSize = par("queueSize").intValue();

    // Initialize statistics
    learningTableSizeVector.setName("learningTableSize");
    floodingRateVector.setName("floodingRate");
    queueLengthVector.setName("queueLength"); // New vector

    msgForwardedSignal = registerSignal("msgForwarded");
    msgFloodedSignal = registerSignal("msgFlooded");
    macLearnedSignal = registerSignal("macLearned");
    learningTableSizeSignal = registerSignal("learningTableSize");

    queueLengthSignal = registerSignal("queueLength");          // Register new signal
    msgsDroppedQueueFullSignal = registerSignal("msgsDroppedQueueFull"); // Register new signal

    // Create self-message for processing completion
    processingCompleteMsg = new cMessage("processingComplete");

    EV << "SpineSwitch " << getIndex() << " initialized with processingDelay="
           << processingDelay << "s, queueSize=" << queueSize << endl;
}

void SpineSwitch::handleMessage(cMessage *msg)
{
    // If it's our self-message, means a message just finished processing
    if (msg == processingCompleteMsg) {
        isProcessing = false; // Processor is now free
        // Record queue length and emit signal (before potentially taking next msg)
        queueLengthVector.record(messageQueue.getLength());
        emit(queueLengthSignal, messageQueue.getLength());

        if (!messageQueue.isEmpty()) {
            // Process the next message from the queue
            processNextMessage();
        }
        return; // Handled self-message, exit
    }

    // Otherwise, it's an incoming data message
    CustomMsg *customMsg = check_and_cast<CustomMsg *>(msg);

    // Check if queue is full
    if (messageQueue.getLength() >= queueSize) {
        messagesDroppedQueueFull++; // Increment new counter
        emit(msgsDroppedQueueFullSignal, 1); // Emit new signal
        EV << "SpineSwitch " << getIndex() << " dropped msg " << customMsg->getMsgId()
           << " - queue full. Current queue size: " << messageQueue.getLength() << endl;
        delete customMsg;
        updateStatistics(); // Update stats even on drop
        return;
    }

    // Message accepted into queue or for immediate processing
    messageQueue.insert(customMsg);
    // Record queue length and emit signal (after inserting incoming msg)
    queueLengthVector.record(messageQueue.getLength());
    emit(queueLengthSignal, messageQueue.getLength());

    if (!isProcessing) {
        // If processor is free, start processing the message immediately
        processNextMessage();
    } else {
        EV << "SpineSwitch " << getIndex() << " queued msg " << customMsg->getMsgId()
           << ". Queue length: " << messageQueue.getLength() << endl;
    }

    updateStatistics();
}

void SpineSwitch::processNextMessage()
{
    if (messageQueue.isEmpty()) {
        isProcessing = false;
        return;
    }

    // Take message from the head of the queue
    CustomMsg *customMsg = check_and_cast<CustomMsg *>(messageQueue.pop());
    isProcessing = true; // Mark processor as busy

    // Schedule processing completion
    scheduleAt(simTime() + processingDelay, processingCompleteMsg);

    // --- Start actual message forwarding/flooding logic (original handleMessage logic) ---
    int arrivalPort = customMsg->getArrivalGate()->getIndex();

    totalMessages++; // Moved totalMessages increment here as message is now being processed

    // Increment hop count
    customMsg->setHopCount(customMsg->getHopCount() + 1);

    // Learn source MAC address
    std::string srcAddr = customMsg->getSrcAddr();
    learnMacAddress(srcAddr, arrivalPort);

    // Forward based on destination MAC address
    std::string dstAddr = customMsg->getDstAddr();
    auto it = macToPort.find(dstAddr);

    if (it != macToPort.end() && it->second != arrivalPort) {
        // Know destination, forward to specific port
        send(customMsg, "out$o", it->second); // OMNeT++ takes ownership of customMsg
        messagesForwarded++;
        emit(msgForwardedSignal, 1);
        EV << "SpineSwitch " << getIndex() << " forwarded msg " << customMsg->getMsgId()
           << " to port " << it->second << " (learned destination) after " << processingDelay << "s processing." << endl;
    } else {

        floodMessage(customMsg, arrivalPort);
        messagesFlooded++;
        emit(msgFloodedSignal, 1);
        EV << "SpineSwitch " << getIndex() << " flooded msg " << customMsg->getMsgId()
           << " (unknown destination) after " << processingDelay << "s processing." << endl;
    }
    // --- End actual message forwarding/flooding logic ---
}


void SpineSwitch::learnMacAddress(const std::string& macAddr, int port)
{
    auto it = macToPort.find(macAddr);
    if (it == macToPort.end()) {
        // New MAC address
        macToPort[macAddr] = port;
        macAddressesLearned++;
        emit(macLearnedSignal, 1);
        emit(learningTableSizeSignal, macToPort.size());
        EV << "SpineSwitch " << getIndex() << " learned MAC " << macAddr
           << " on port " << port << endl;
    } else if (it->second != port) {
        // MAC address moved to different port
        it->second = port;
        EV << "SpineSwitch " << getIndex() << " updated MAC " << macAddr
           << " to port " << port << endl;
    }
}

void SpineSwitch::floodMessage(CustomMsg *msg, int arrivalPort)
{
    int numPorts = gateSize("out$o");
    bool originalMsgSent = false; // Flag to ensure the original message is sent exactly once

    for (int i = 0; i < numPorts; i++) {
        if (i != arrivalPort) {
            if (!originalMsgSent) {
                // Send the original message on the first available non-arrival gate
                send(msg, "out$o", i); // OMNeT++ now owns 'msg'
                originalMsgSent = true;
            } else {
                // For subsequent gates, duplicate the message and send the copy
                CustomMsg *copy = msg->dup();
                send(copy, "out$o", i); // OMNeT++ owns 'copy'
            }
        }
    }

    if (!originalMsgSent) {
        delete msg; // Clean up the original message if it wasn't sent anywhere
    }
}

void SpineSwitch::updateStatistics()
{
    learningTableSizeVector.record(macToPort.size());

    // Calculate flooding rate (percentage of messages that are flooded)
    if (totalMessages > 0) {
        double floodingRate = (double)messagesFlooded / totalMessages;
        floodingRateVector.record(floodingRate);
    }
}

void SpineSwitch::finish()
{
    EV << "SpineSwitch " << getIndex() << " Statistics:" << endl;
    EV << "  Total Messages Processed: " << totalMessages << endl;
    EV << "  Messages Forwarded: " << messagesForwarded << endl;
    EV << "  Messages Flooded: " << messagesFlooded << endl;
    EV << "  MAC Addresses Learned: " << macAddressesLearned << endl;
    EV << "  Learning Table Size: " << macToPort.size() << endl;
    EV << "  Messages Dropped (Queue Full): " << messagesDroppedQueueFull << endl; // New scalar
    EV << "  Final Queue Length: " << messageQueue.getLength() << endl; // New scalar


    recordScalar("totalMessagesProcessed", totalMessages);
    recordScalar("messagesForwarded", messagesForwarded);
    recordScalar("messagesFlooded", messagesFlooded);
    recordScalar("macAddressesLearned", macAddressesLearned);
    recordScalar("finalLearningTableSize", macToPort.size());
    recordScalar("messagesDroppedQueueFull", messagesDroppedQueueFull); // New scalar
    recordScalar("finalQueueLength", messageQueue.getLength()); // New scalar

    if (totalMessages > 0) {
        double floodingRate = (double)messagesFlooded / totalMessages;
        recordScalar("floodingRate", floodingRate);
        recordScalar("forwardingRate", (double)messagesForwarded / totalMessages);
    }

    // Record learning efficiency
    if (macAddressesLearned > 0) {
        recordScalar("learningEfficiency", (double)messagesForwarded / macAddressesLearned);
    }
}
