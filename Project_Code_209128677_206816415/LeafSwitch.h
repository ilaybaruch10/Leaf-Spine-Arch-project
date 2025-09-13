#ifndef __LEAFSWITCH_H_
#define __LEAFSWITCH_H_

#include <omnetpp.h>
#include <map>
#include <vector>
#include <string>
#include "CustomMsg_m.h"

using namespace omnetpp;

class LeafSwitch : public cSimpleModule
{
private:
    std::map<std::string, int> hostMacToPort; // MAC address to port mapping for directly connected hosts
    std::vector<int> spinePorts; // Ports connected to spine switches
    std::vector<int> hostPorts;  // Ports connected to hosts

    // Processing and Queueing
    cQueue messageQueue;            // Queue for incoming messages
    bool isProcessing;              // True if currently processing a message
    cMessage *processingCompleteMsg; // Self-message to signal processing completion
    double processingDelay;         // Parameter: processing time per message
    int queueSize;                  // Parameter: max queue length

    // Statistics
    cOutVector throughputVector;
    cOutVector loadBalanceVector;
    cOutVector queueLengthVector; // New: records queue size over time

    simsignal_t msgForwardedToHostSignal;
    simsignal_t msgForwardedToSpineSignal;
    simsignal_t msgDroppedSignal;
    simsignal_t throughputSignal;
    simsignal_t queueLengthSignal;          // New signal
    simsignal_t msgsDroppedQueueFullSignal; // New signal

    // Counters
    int messagesFromHosts;
    int messagesFromSpines;
    int messagesToHosts;
    int messagesToSpines;
    int messagesDropped;
    int messagesDroppedQueueFull; // New counter
    std::map<int, int> spinePortUsage; // Track usage per spine port for load balancing analysis

public:
    LeafSwitch();
    virtual ~LeafSwitch();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

private:
    void learnHostLocation(const std::string& macAddr, int port);
    int selectRandomSpinePort();
    bool isSpinePort(int port);
    bool isHostPort(int port);
    void updateThroughputStats();
    void processNextMessage(); // New helper function
};

#endif
