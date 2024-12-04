#pragma once
#include "../Node.hpp"


class C2_Node : public Node {

public :
    C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex) {

            initializeTransitionMap();
            setInitialState(NodeState::Sleeping);

    };

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



    #else
        #error "Unknown COMMUNICATION_PERIOD mode"
    #endif

};
