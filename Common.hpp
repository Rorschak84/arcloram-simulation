#pragma once


// Define modes as preprocessor macros
#define COMMUNICATION_PERIOD RRC_BEACON
#define TOPOLOGY Line

// Conditionally compile based on COMMUNICATION_PERIOD
#if COMMUNICATION_PERIOD == RRC_BEACON
    const char* communicationMode = "RRC_Beacon";
    //TODO: eventually define others parameters of mode here, such as the max min number of beacon packets
#elif COMMUNICATION_PERIOD == ENC_BEACON
    const char* communicationMode = "ENC_Beacon";
#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif

// Conditionally compile based on TOPOLOGY
#if TOPOLOGY == Line
    const char* topology = "Line";
#elif TOPOLOGY == Star
    const char* topology = "Star";
#else
    #error "Unknown TOPOLOGY mode"
#endif
/*
                USE CASES:
There is two major components:
-the topology (position of nodes)
-the Communication Window (Ex: RRC_Beacon, RRC_Downlink, ENC_Beacon), that will dictate the classes of nodes



The seed class will be used to define the topology and the communication window, automatically provisioning the required elements to show the functionnalities of the protocol


Use cases Description:
    For RRC:
            -Topologies:
                -Line
                -Star
                -Mesh

            -Communication Window:
                -RRC_Beacon
                -RRC_Downlink (Or RRC_Flood, neet to set a name for good)
                -RRC_Uplink  (same)   
    For ENC:
            -Topologies:
                -Star 
            
            -Communication Window:
                -ENC_Beacon
                -ENC_Downlink
                -ENC_Uplink

If time allows, we will consider an hybrid use case that will combine the two protocols



Trick: //TODO: use the math online tool that was free when in college to generate the coordinates of the nodes (they have to be in range)

*/  
