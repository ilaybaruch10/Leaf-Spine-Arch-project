#ifndef __CUSTOMHOST_H_
#define __CUSTOMHOST_H_

#include <omnetpp.h>
#include <vector>
#include <string>
#include "CustomMsg_m.h"

using namespace omnetpp;

class CustomHost : public cSimpleModule
{
private:
    std::string myMacAddr;
    std::vector<std::string> allHostMacAddrs;
    double msgGenerationRate;
    int msgIdCounter;
    int seqNumCounter;

    // Statistics
    cOutVector endToEndDelayVector;
    cOutVector msgSizeVector;
    cOutVector hopCountVector;

    simsignal_t msgSentSignal;
    simsignal_t msgReceivedSignal;
    simsignal_t msgDroppedSignal;
    simsignal_t endToEndDelaySignal;

    // Counters
    int messagesSent;
    int messagesReceived;
    int messagesDropped;

    // For calculating averages
    double totalEndToEndDelay;
    double totalMsgSize;
    double totalHopCount;

public:
    CustomHost();
    virtual ~CustomHost();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

private:
    void generateMessage();
    void scheduleNextMessage();
    std::string selectRandomDestination();
    std::string generateMacAddress(int hostIndex);
};

#endif
