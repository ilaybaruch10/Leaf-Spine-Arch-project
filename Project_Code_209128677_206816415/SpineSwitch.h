#ifndef __SPINESWITCH_H_
#define __SPINESWITCH_H_

#include <omnetpp.h>
#include <map>
#include <string>
#include "CustomMsg_m.h"

using namespace omnetpp;

class SpineSwitch : public cSimpleModule
{
private:
    std::map<std::string, int> macToPort; // Learning table: MAC address to port mapping


    // Processing and Queueing
    cQueue messageQueue;            // Queue for incoming messages
    bool isProcessing;              // True if currently processing a message
    cMessage *processingCompleteMsg; // Self-message to signal processing completion
    double processingDelay;         // Parameter: processing time per message
    int queueSize;


    // Statistics
    cOutVector learningTableSizeVector;
    cOutVector floodingRateVector;
    cOutVector queueLengthVector; // New: records queue size over time

    simsignal_t msgForwardedSignal;
    simsignal_t msgFloodedSignal;
    simsignal_t macLearnedSignal;
    simsignal_t learningTableSizeSignal;
    simsignal_t queueLengthSignal;          // New signal
    simsignal_t msgsDroppedQueueFullSignal; // New signal

    // Counters
    int messagesForwarded;
    int messagesFlooded;
    int macAddressesLearned;
    int totalMessages;
    int messagesDroppedQueueFull; // New counter (Spine can also drop due to full queue)

public:
    SpineSwitch();
    virtual ~SpineSwitch();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

private:
    void learnMacAddress(const std::string& macAddr, int port);
    void forwardMessage(CustomMsg *msg, int port);
    void floodMessage(CustomMsg *msg, int arrivalPort);
    void processNextMessage(); // New helper function
    void updateStatistics();
};

#endif
