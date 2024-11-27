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

    void receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir) override;

    #if COMMUNICATION_PERIOD == RRC_BEACON

        bool shouldSendBeacon=false;
        short hopCount=-1;
        std::vector<int> beaconSlots;
         
        int computeRandomNbBeaconPackets();
        //selecte m slots randomly in the n slots, and return an orderred list of the selected slots
        std::vector<int> selectRandomSlots(int m);

        bool canNodeReceiveMessage();
        bool isTransmittingWhileCommunicating=false;


    #else
        #error "Unknown COMMUNICATION_PERIOD mode"
    #endif
};
