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



#if COMMUNICATION_PERIOD == RRC_BEACON
    std::vector<int> beaconSlots; // ex: {0,  3, 4, 9} -> beacon to send now, in three slots, in four slots, in nine slots
    bool shouldSendBeacon=true;


    int minimumNbBeaconPackets=2;//TODO: add in common, those are paremeters that we will change to test our protocol?
    int maximumNbBeaconPackets=4;
    int nbSlotsPossibleForOneBeacon=10;



    int computeRandomNbBeaconPackets() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(minimumNbBeaconPackets, maximumNbBeaconPackets);
        return dis(gen);
    }

    //selecte m slots randomly in the n slots, and return an orderred list of the selected slots
    std::vector<int> selectRandomSlots(int m) {
    
        // Step 1: Create a vector of slots [0, 1, ..., n-1]
        std::vector<int> slots(nbSlotsPossibleForOneBeacon);
        for (int i = 0; i < nbSlotsPossibleForOneBeacon; ++i) {
            slots[i] = i;
        }

        // Step 2: Shuffle the vector randomly
        std::random_device rd;  // Seed for random number generator
        std::mt19937 rng(rd()); // Mersenne Twister RNG
        std::shuffle(slots.begin(), slots.end(), rng);

        // Step 3: Take the first m slots
        std::vector<int> selected(slots.begin(), slots.begin() + m);
        
        // Step 4: Sort the selected slots in ascending order
        std::sort(selected.begin(), selected.end());

        return selected;
    }


#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif


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

};
