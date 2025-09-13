
// CustomMsg.msg
message CustomMsg {
    string srcAddr;       // Source MAC address
    string dstAddr;       // Destination MAC address
    simtime_t sentAt;     // When message was sent
    simtime_t receivedAt; // When message was received
    int msgId;            // Unique message identifier
    int msgSize;          // Message size in bytes
    int hopCount;         // Number of hops taken
    int seqNum;           // Sequence number for ordering
}