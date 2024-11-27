#pragma once
#include "../Node.hpp"
#include "../Common.hpp"

class C3_Node : public Node {

public :
    C3_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex) {

            initializeTransitionMap();                 
            setInitialState(NodeState::Sleeping);
    };

    int getClassId() const  {
        return 3;
    }
    std::string initMessage() const override;






protected:


#if COMMUNICATION_PERIOD == RRC_BEACON
    std::vector<int> beaconSlots; // ex: {0,  3, 4, 9} -> beacon to send now, in three slots, in four slots, in nine slots
    bool shouldSendBeacon=true;





    int computeRandomNbBeaconPackets();

    //selecte m slots randomly in the n slots, and return an orderred list of the selected slots
    std::vector<int> selectRandomSlots(int m);

#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif



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

};
