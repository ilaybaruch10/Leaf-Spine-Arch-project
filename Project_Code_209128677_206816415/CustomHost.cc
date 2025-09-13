#include "CustomHost.h"

Define_Module(CustomHost);

CustomHost::CustomHost()
{
    msgIdCounter = 0;
    seqNumCounter = 0;
    messagesSent = 0;
    messagesReceived = 0;
    messagesDropped = 0;
    totalEndToEndDelay = 0.0;
    totalMsgSize = 0.0;
    totalHopCount = 0.0;
}

CustomHost::~CustomHost()
{
}

void CustomHost::initialize()
{
    // Get parameters
    msgGenerationRate = par("msgGenerationRate").doubleValue();
    int numHosts = getSystemModule()->par("numHosts").intValue();
    int hostIndex = getIndex();

    // Generate MAC address for this host
    myMacAddr = generateMacAddress(hostIndex);

    // Generate MAC addresses for all hosts
    for (int i = 0; i < numHosts; i++) {
        allHostMacAddrs.push_back(generateMacAddress(i));
    }

    // Initialize statistics
    endToEndDelayVector.setName("endToEndDelay");
    msgSizeVector.setName("messageSize");
    hopCountVector.setName("hopCount");

    msgSentSignal = registerSignal("msgSent");
    msgReceivedSignal = registerSignal("msgReceived");
    msgDroppedSignal = registerSignal("msgDropped");
    endToEndDelaySignal = registerSignal("endToEndDelay");

    // Schedule first message generation
    scheduleNextMessage();

    EV << "CustomHost " << hostIndex << " initialized with MAC: " << myMacAddr << endl;
}

void CustomHost::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        // Time to generate a new message
        generateMessage();
        scheduleNextMessage();
        delete msg;
    } else {
        // Received a message from network
        CustomMsg *customMsg = check_and_cast<CustomMsg *>(msg);
        customMsg->setReceivedAt(simTime());

        if (customMsg->getDstAddr() == myMacAddr) {
            // Message is for this host
            messagesReceived++;
            simtime_t delay = simTime() - customMsg->getSentAt();
            EV << "sim time = " << simTime() << "message sent at:" << customMsg->getSentAt() << "delay= " << delay << endl;

            EV << "Host " << getIndex() << " received message " << customMsg->getMsgId()
               << " from " << customMsg->getSrcAddr() << " with delay " << delay << "s" << endl;

            // Record statistics
            endToEndDelayVector.record(delay);
            msgSizeVector.record(customMsg->getMsgSize());
            hopCountVector.record(customMsg->getHopCount());

            // Update totals for average calculation
            totalEndToEndDelay += SIMTIME_DBL(delay);
            totalMsgSize += customMsg->getMsgSize();
            totalHopCount += customMsg->getHopCount();

            emit(msgReceivedSignal, 1);
            emit(endToEndDelaySignal, delay);
        } else {
            // Message not for this host - drop it
            messagesDropped++;
            EV << "Host " << getIndex() << " dropped message " << customMsg->getMsgId()
               << " (intended for " << customMsg->getDstAddr() << ")" << endl;
            emit(msgDroppedSignal, 1);
        }
        delete msg;
    }
}

void CustomHost::generateMessage()
{
    CustomMsg *msg = new CustomMsg("dataMsg");

    // Set message fields
    msg->setSrcAddr(myMacAddr.c_str());
    msg->setDstAddr(selectRandomDestination().c_str());
    msg->setSentAt(simTime());
    msg->setMsgId(++msgIdCounter);
    msg->setSeqNum(++seqNumCounter);
    msg->setMsgSize(uniform(64, 1500)); // Random message size
    msg->setHopCount(0);

    messagesSent++;
    emit(msgSentSignal, 1);

    EV << "Host " << getIndex() << " generated message " << msg->getMsgId()
       << " to " << msg->getDstAddr() << endl;

    send(msg, "out$o");
}

void CustomHost::scheduleNextMessage()
{
    // Schedule next message generation using exponential distribution
    simtime_t nextTime = simTime() + exponential(1.0 / msgGenerationRate);
    scheduleAt(nextTime, new cMessage("generateMsg"));
}

std::string CustomHost::selectRandomDestination()
{
    // Select random destination (excluding self)
    std::string dest;
    do {
        int randIndex = intuniform(0, allHostMacAddrs.size() - 1);
        dest = allHostMacAddrs[randIndex];
    } while (dest == myMacAddr);

    return dest;
}

std::string CustomHost::generateMacAddress(int hostIndex)
{
    char macAddr[18];
    sprintf(macAddr, "00:00:00:00:%02X:%02X", hostIndex / 256, hostIndex % 256);
    return std::string(macAddr);
}

void CustomHost::finish()
{
    EV << "Host " << getIndex() << " Statistics:" << endl;
    EV << "  Messages Sent: " << messagesSent << endl;
    EV << "  Messages Received: " << messagesReceived << endl;
    EV << "  Messages Dropped: " << messagesDropped << endl;

    recordScalar("messagesSent", messagesSent);
    recordScalar("messagesReceived", messagesReceived);
    recordScalar("messagesDropped", messagesDropped);

    if (messagesReceived > 0) {
        double avgEndToEndDelay = totalEndToEndDelay / messagesReceived;
        double avgMsgSize = totalMsgSize / messagesReceived;
        double avgHopCount = totalHopCount / messagesReceived;

        recordScalar("avgEndToEndDelay", avgEndToEndDelay);
        recordScalar("avgMsgSize", avgMsgSize);
        recordScalar("avgHopCount", avgHopCount);

        EV << "  Average End-to-End Delay: " << avgEndToEndDelay << "s" << endl;
        EV << "  Average Message Size: " << avgMsgSize << " bytes" << endl;
        EV << "  Average Hop Count: " << avgHopCount << endl;
    }
}
