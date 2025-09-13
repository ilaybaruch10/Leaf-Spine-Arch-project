#include "LeafSwitch.h"
#include <algorithm>

Define_Module(LeafSwitch);

LeafSwitch::LeafSwitch()
{
    messagesFromHosts = 0;
    messagesFromSpines = 0;
    messagesToHosts = 0;
    messagesToSpines = 0;
    messagesDropped = 0;

    messagesDroppedQueueFull = 0; // Initialize new counter
    isProcessing = false;
    processingCompleteMsg = nullptr; // Initialize self-message pointer
}

LeafSwitch::~LeafSwitch()
{
    cancelAndDelete(processingCompleteMsg); // Clean up self-message if still scheduled
}

void LeafSwitch::initialize()
{
    // Get parameters
    processingDelay = par("processingDelay").doubleValue();
    queueSize = par("queueSize").intValue();

    // Get topology parameters
    int numHosts = getSystemModule()->par("numHosts").intValue();
    int numSpines = getSystemModule()->par("numSpines").intValue();
    int hostsPerLeaf = getSystemModule()->par("hostsPerLeaf").intValue();

    // Determine which ports are connected to hosts vs spines
    // Assuming first hostsPerLeaf ports are for hosts, remaining for spines
    for (int i = 0; i < hostsPerLeaf; i++) {
        hostPorts.push_back(i);
    }
    for (int i = hostsPerLeaf; i < hostsPerLeaf + numSpines; i++) {
        spinePorts.push_back(i);
        spinePortUsage[i] = 0; // Initialize usage counter
    }

    // Initialize statistics
    throughputVector.setName("throughput");
    loadBalanceVector.setName("loadBalance");
    queueLengthVector.setName("queueLength"); // New vector

    msgForwardedToHostSignal = registerSignal("msgForwardedToHost");
    msgForwardedToSpineSignal = registerSignal("msgForwardedToSpine");
    msgDroppedSignal = registerSignal("msgDropped");
    throughputSignal = registerSignal("throughput");

    queueLengthSignal = registerSignal("queueLength"); // Register new signal
    msgsDroppedQueueFullSignal = registerSignal("msgsDroppedQueueFull"); // Register new signal

    // Create self-message for processing completion
    processingCompleteMsg = new cMessage("processingComplete");

    EV << "LeafSwitch " << getIndex() << " initialized with " << hostPorts.size()
       << " host ports and " << spinePorts.size() << " spine ports, processingDelay= "<< processingDelay << "s, queueSize=" << queueSize << endl;
}

void LeafSwitch::handleMessage(cMessage *msg)
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
        messagesDropped++;
        messagesDroppedQueueFull++; // Increment new counter
        emit(msgDroppedSignal, 1);
        emit(msgsDroppedQueueFullSignal, 1); // Emit new signal
        EV << "LeafSwitch " << getIndex() << " dropped msg " << customMsg->getMsgId()
           << " - queue full. Current queue size: " << messageQueue.getLength() << endl;
        delete customMsg;
        updateThroughputStats(); // Update stats even on drop
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
        EV << "LeafSwitch " << getIndex() << " queued msg " << customMsg->getMsgId()
           << ". Queue length: " << messageQueue.getLength() << endl;
    }

    updateThroughputStats();
}


void LeafSwitch::processNextMessage()
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

    // --- Start actual message forwarding logic (original handleMessage logic) ---
    int arrivalPort = customMsg->getArrivalGate()->getIndex();

    // Increment hop count
    customMsg->setHopCount(customMsg->getHopCount() + 1);

    // Learn source MAC address location if from host
    if (isHostPort(arrivalPort)) {
        learnHostLocation(customMsg->getSrcAddr(), arrivalPort);
        messagesFromHosts++;
    } else {
        messagesFromSpines++;
    }

    std::string dstAddr = customMsg->getDstAddr();

    if (isHostPort(arrivalPort)) {
        // Message from host
        auto it = hostMacToPort.find(dstAddr);
        if (it != hostMacToPort.end()) {
            // Destination is directly connected host
            send(customMsg, "out$o", it->second);
            messagesToHosts++;
            emit(msgForwardedToHostSignal, 1);
            EV << "LeafSwitch " << getIndex() << " forwarded msg " << customMsg->getMsgId()
               << " to directly connected host on port " << it->second << " after " << processingDelay << "s processing." << endl;
        } else {
            // Send to random spine
            int spinePort = selectRandomSpinePort();
            send(customMsg, "out$o", spinePort);
            messagesToSpines++;
            spinePortUsage[spinePort]++;
            emit(msgForwardedToSpineSignal, 1);
            EV << "LeafSwitch " << getIndex() << " forwarded msg " << customMsg->getMsgId()
               << " to spine on port " << spinePort << " after " << processingDelay << "s processing." << endl;
        }
    } else {
        // Message from spine
        auto it = hostMacToPort.find(dstAddr);
        if (it != hostMacToPort.end()) {
            // Destination is directly connected host
            send(customMsg, "out$o", it->second);
            messagesToHosts++;
            emit(msgForwardedToHostSignal, 1);
            EV << "LeafSwitch " << getIndex() << " forwarded msg " << customMsg->getMsgId()
               << " from spine to host on port " << it->second << " after " << processingDelay << "s processing." << endl;
        } else {
            // Drop message - destination not directly connected (this logic remains as before)
            messagesDropped++;
            emit(msgDroppedSignal, 1);
            EV << "LeafSwitch " << getIndex() << " dropped msg " << customMsg->getMsgId()
               << " - destination not directly connected (after processing)" << endl;
            delete customMsg; // Delete the message if it's dropped here
        }
    }
    // --- End actual message forwarding logic ---
}


void LeafSwitch::learnHostLocation(const std::string& macAddr, int port)
{
    auto it = hostMacToPort.find(macAddr);
    if (it == hostMacToPort.end()) {
        hostMacToPort[macAddr] = port;
        EV << "LeafSwitch " << getIndex() << " learned MAC " << macAddr
           << " is on port " << port << endl;
    }
}

int LeafSwitch::selectRandomSpinePort()
{
    int randIndex = intuniform(0, spinePorts.size() - 1);
    return spinePorts[randIndex];
}

bool LeafSwitch::isSpinePort(int port)
{
    for (int spinePort : spinePorts) {
        if (spinePort == port) return true;
    }
    return false;
}

bool LeafSwitch::isHostPort(int port)
{
    for (int hostPort : hostPorts) {
        if (hostPort == port) return true;
    }
    return false;
}

void LeafSwitch::updateThroughputStats()
{

    int totalMessagesHandled = messagesFromHosts + messagesFromSpines + messagesDroppedQueueFull; // Consider also messages dropped at queue
    throughputVector.record(totalMessagesHandled);
    emit(throughputSignal, totalMessagesHandled);

    // Calculate load balance metric (coefficient of variation of spine port usage)
    if (!spinePortUsage.empty()) {
        double sum = 0, sumSq = 0;
        for (auto& pair : spinePortUsage) {
            sum += pair.second;
            sumSq += pair.second * pair.second;
        }
        double mean = sum / spinePortUsage.size();
        if (mean > 0) {
            double variance = (sumSq / spinePortUsage.size()) - (mean * mean);
            double cv = (mean == 0) ? 0 : sqrt(variance) / mean; // Handle mean = 0 to avoid division by zero
            loadBalanceVector.record(cv);
        } else {
            loadBalanceVector.record(0); // If mean is 0, CV is 0 (perfect balance with no traffic)
        }
    }
}

void LeafSwitch::finish()
{
    EV << "LeafSwitch " << getIndex() << " Statistics:" << endl;
    EV << "  Messages from Hosts: " << messagesFromHosts << endl;
    EV << "  Messages from Spines: " << messagesFromSpines << endl;
    EV << "  Messages to Hosts: " << messagesToHosts << endl;
    EV << "  Messages to Spines: " << messagesToSpines << endl;
    EV << "  Total Messages Dropped (includes queue full): " << messagesDropped << endl;
    EV << "  Messages Dropped (Queue Full): " << messagesDroppedQueueFull << endl; // New scalar
    EV << "  Learned MAC addresses: " << hostMacToPort.size() << endl;
    EV << "  Final Queue Length: " << messageQueue.getLength() << endl; // New scalar

    recordScalar("messagesFromHosts", messagesFromHosts);
    recordScalar("messagesFromSpines", messagesFromSpines);
    recordScalar("messagesToHosts", messagesToHosts);
    recordScalar("messagesToSpines", messagesToSpines);
    recordScalar("totalMessagesDropped", messagesDropped);
    recordScalar("messagesDroppedQueueFull", messagesDroppedQueueFull); // New scalar
    recordScalar("learnedMacAddresses", hostMacToPort.size());
    recordScalar("finalQueueLength", messageQueue.getLength()); // New scalar

    // Record spine port usage for load balancing analysis
    for (auto& pair : spinePortUsage) {
        recordScalar(("spinePort" + std::to_string(pair.first) + "Usage").c_str(), pair.second);
    }
}
