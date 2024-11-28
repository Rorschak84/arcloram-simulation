#ifndef COMMON_HPP
#define COMMON_HPP

#include <unordered_map>
#include <string>


 namespace common{




/*
*  Define General Parameters for the Simulation 
*   Define the Topology Desired and the Communication Period Mode in the two preprocessor Macros
*   Some parameters specific to the communication mode and the topology are defined in the corresponding sections
*/



/*
 ------------------------------------USE CASES-----------------------------------
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
                -RRC_Downlink 
                -RRC_Uplink    
    For ENC:
            -Topologies:
                -Star 
            
            -Communication Window:
                -ENC_Beacon
                -ENC_Downlink
                -ENC_Uplink

If time allows, we will consider an hybrid use case that will combine the two protocols

*/  


//-----------------------------------------GENERAL PARAMETERS-----------------------------------------
constexpr const int tickIntervalForClock_ms=100; // The tick interval should be at most equal to the minimum duration of all the windows, divided by 4/5, otherwise unpredictable behavior may occur
constexpr const int baseTimeOffset=1000;
constexpr const double distanceThreshold=1000;



//-----------------------------------------Communication Mode and  Topology-----------------------------------------

#define COMMUNICATION_PERIOD RRC_BEACON
#define TOPOLOGY Line

#if COMMUNICATION_PERIOD == RRC_BEACON

    constexpr const char* communicationMode = "RRC_Beacon";

    //For the Protocol Stack in Nodes
    constexpr const int minimumNbBeaconPackets=2;
    constexpr const int maximumNbBeaconPackets=4;
    constexpr const int nbSlotsPossibleForOneBeacon=10;
    constexpr const int guardTime=50; //a sufficient guard time is needed to be sure every nodes that should are able to receive messages
    constexpr const int typePacket=0x01;
    constexpr const int timeOnAirBeacon=70;//This should be quite high. Indeed, we don't have transmit from every nodes, and then receives for every paclets for every nodes, by having a great TOA we bypass this limitation TODO: change this to have a more resilient simulation 
    
    //For the Time Division Multiple Access Scheme in Seed
    constexpr  const unsigned int lengthTransmissionWindow = 500;
    constexpr  const  unsigned int lengthSleepingWindow = 1000;
    constexpr  const  unsigned int nbComWindows =40;




    //For the Packet
    // Define field positions and sizes in the packet

    //TODO:
    //Put every variables (index + nbBytes) of the fields as consexpr
    //they are used in some function


    // Format: { "field_name", {start_index, size_in_bytes} }
    inline const std::unordered_map<std::string, std::pair<size_t, size_t>> fieldMap = {
            {"type", {0, 1}},             // "type" starts at index 0, 1 byte long
            {"timeStamp", {1, 4}},       // "timeStamp" starts at index 1, 4 bytes long
            {"costFunction", {5, 1}},    // "costFunction" starts at index 5, 1 byte long
            {"hopCount", {6, 2}},        // "hopCount" starts at index 6, 2 bytes long
            {"globalIDPacket", {8, 2}},  // "globalIDPacket" starts at index 8, 2 bytes long
            {"senderGlobalId", {10, 2}}, // "senderGlobalId" starts at index 10, 2 bytes long
            {"hashFunction", {12, 4}}    // "hashFunction" starts at index 12, 4 bytes long
        };


#elif COMMUNICATION_PERIOD == ENC_BEACON
    const char* communicationMode = "ENC_Beacon";
#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif


#if TOPOLOGY == Line
    constexpr const char* topology = "Line";
#elif TOPOLOGY == Star
    const char* topology = "Star";
#else
    #error "Unknown TOPOLOGY mode"
#endif


 }
#endif // COMMON_HPP