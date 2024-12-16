#pragma once
#include "../Node.hpp"


class C2_Node : public Node {

public :

#if COMMUNICATION_PERIOD == RRC_DOWNLINK|| COMMUNICATION_PERIOD == RRC_BEACON
    C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex) {

            initializeTransitionMap();
            setInitialState(NodeState::Sleeping);

    };
#elif COMMUNICATION_PERIOD == RRC_UPLINK
    C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,uint16_t nextNodeIdInPath,uint8_t hopCount)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex), nextNodeIdInPath(nextNodeIdInPath), hopCount(hopCount) {

            initializeTransitionMap();
            setInitialState(NodeState::Sleeping);

    };

#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif
    int getClassId() const  {
        return 2;
    }
    
    std::string initMessage() const override;
 
    protected:


    bool canTransmitFromListening();
    bool canTransmitFromSleeping();
    bool canTransmitFromTransmitting();
    bool canTransmitFromCommunicating();

    bool canListenFromTransmitting();
    bool canListenFromSleeping();
    bool canListenFromListening();
    bool canListenFromCommunicating();

    bool canSleepFromTransmitting();
    bool canSleepFromListening();
    bool canSleepFromSleeping();
    bool canSleepFromCommunicating();

    bool canCommunicateFromTransmitting();
    bool canCommunicateFromListening();
    bool canCommunicateFromSleeping();
    bool canCommunicateFromCommunicating();

    bool receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir) override;


    #if COMMUNICATION_PERIOD == RRC_DOWNLINK

    bool shouldSendBeacon=false;
    std::vector<int> beaconSlots; 
    std::vector<uint16_t> globalIDPacketList;//
    bool canNodeReceiveMessage();
    bool isTransmittingWhileCommunicating=false;   

    //this variable contains the id of the node that is the final receiver of the flooding packet circulating in the network
    //use to bridge the receive function and the transmit function
    std::optional<uint32_t> packetFinalReceiverId ;


    #elif COMMUNICATION_PERIOD == RRC_BEACON

        bool shouldSendBeacon=false;
        std::optional<uint8_t> hopCount;
        std::optional<uint32_t> lastTimeStampReceived;
        double battery= 80.0; //in percentage
        std::vector<int> beaconSlots; 
        std::vector<uint16_t> globalIDPacketList;//
        std::optional<uint8_t> pathCost;
        std::optional<uint16_t> nextNodeIdInPath;
        bool canNodeReceiveMessage();
        bool isTransmittingWhileCommunicating=false;

    #elif COMMUNICATION_PERIOD== RRC_UPLINK
        //Reception
        bool canNodeReceiveMessage();
        bool isTransmittingWhileCommunicating=false;
        bool isDataWindow=false;

        //Variables that should have been provided during beacon phase
        uint16_t nextNodeIdInPath;
        uint8_t hopCount;

        //Data Strategy
        std::vector<int> transmissionSlots; //the slots where the node WILL transmit (unless if no data to send), it's fixed
        uint8_t nbPayloadLeft; //the number of payload left to send(initial + forward packet)(represents the data that will be sent, in the simulation, every payload is the same (0xFF...FF)) 
        uint8_t initialnbPaylaod=2; //initial number of payload
        using SenderID = uint16_t;
        using PacketID = uint16_t;
        using PacketList = std::vector<PacketID>; // Alias for a list of Packet IDs
        using PacketMap = std::unordered_map<SenderID, PacketList>; //we need the packet Map to not forward already forwarded data packet
        // Adds a Packet ID to the sender's list
        PacketMap packetsMap; // Data structure to store packets
        void addPacket(SenderID sender, PacketID packet);

        //ACK strategy
        bool shouldReplyACK=false;
        uint16_t lastSenderId;
        uint16_t lastLocalIDPacket;       

    #else
        #error "Unknown COMMUNICATION_PERIOD mode"
    #endif

};
