import inet.common.misc.ThruputMeteringChannel;
//
// Custom Host Module
//
simple CustomHost
{
    parameters:
        double msgGenerationRate = default(1.0);
        @signal[msgSent](type=int);
        @signal[msgReceived](type=int);
        @signal[msgDropped](type=int);
        @signal[endToEndDelay](type=simtime_t);
        //@display("i=device/pc");
    gates:
        inout out;
}

//
// Leaf Switch Module  
//
simple LeafSwitch
{
    parameters:
        double processingDelay = default(0.001); // ms processing time
        int queueSize = default(100);           // Max messages in queue

        @signal[msgForwardedToSpine](type=int);
        @signal[throughput](type=int);
        //@signal[loadBalance](type=int);
        @signal[msgForwardedToHost](type=int);
        @signal[msgDropped](type=int);


    gates:
        inout out[];
}

//
// Spine Switch Module
//
simple SpineSwitch
{
    parameters:
        double processingDelay = default(0.001); // 1ms processing time
        int queueSize = default(100);           // Max messages in queue
        @signal[queueLength](type=int);         // New signal for queue length
        @signal[msgsDroppedQueueFull](type=int); // New signal for dropped due to full queue


        @signal[msgForwarded](type=int);
        @signal[msgFlooded](type=int);
        @signal[macLearned](type=int);
        @signal[learningTableSize](type=unsigned int);

    gates:
        inout out[];
}

//
// Main Network Definition
//
network LeafSpineNetwork
{
    parameters:
        int numHosts = default(16);
        int numLeaves = default(4);
        int numSpines = default(2);
        int hostsPerLeaf = default(4);

        @display("bgb=3757,598");

    submodules:
        // Hosts
        host[numHosts]: CustomHost {
            @display("i=device/laptop;p=${200 + 70*index}, 400");
        }

        // Leaf Switches
        leaf[numLeaves]: LeafSwitch {
            @display("i=block/switch;p=${500 + 100*index}, 250");
            gates:
                out[parent.hostsPerLeaf + parent.numSpines];
        }

        // Spine Switches  
        spine[numSpines]: SpineSwitch {
            @display("i=device/switch;p=${600 + 100*index}, 60");
            gates:
                out[parent.numLeaves];
        }

    connections:
        // Connect hosts to leaf switches
        for i=0..numHosts-1 {
            host[i].out <--> ThruputMeteringChannel {  delay = 0.1 ms; } <--> leaf[int(i/hostsPerLeaf)].out[i%hostsPerLeaf];
        }

        // Connect leaf switches to spine switches (full mesh)
        for i=0..numLeaves-1, for j=0..numSpines-1 {
            leaf[i].out[hostsPerLeaf + j] <--> ThruputMeteringChannel {  delay = 0.1 ms; } <--> spine[j].out[i];
        }
}
