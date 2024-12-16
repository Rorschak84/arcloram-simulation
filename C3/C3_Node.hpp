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

    int getClassId() const override {
        return 3;
    }
    std::string initMessage() const override;

    bool receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir) override;






protected:


#if COMMUNICATION_PERIOD == RRC_BEACON

    std::vector<int> beaconSlots; // ex: {0,  3, 4, 9} -> beacon to send now, in three slots, in four slots, in nine slots
    bool shouldSendBeacon=true;

#elif COMMUNICATION_PERIOD == RRC_DOWNLINK

    std::vector<int> beaconSlots;
    bool shouldSendBeacon=true;

#elif COMMUNICATION_PERIOD== RRC_UPLINK

    bool canNodeReceiveMessage();
    bool shouldReplyACK=false;
    uint16_t lastSenderId;
    uint16_t lastLocalIDPacket;

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
