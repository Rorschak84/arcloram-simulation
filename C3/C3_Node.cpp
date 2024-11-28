#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}


#if COMMUNICATION_PERIOD == RRC_BEACON

    //-----------------------utilities------------------
    int C3_Node::computeRandomNbBeaconPackets() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(minimumNbBeaconPackets, maximumNbBeaconPackets);
        return dis(gen);
    }
    std::vector<int> C3_Node::selectRandomSlots(int m) {
    
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

    //---------------------------state Transition--------------------
    bool C3_Node::canTransmitFromSleeping() { 

        currentState=NodeState::Transmitting;
        // Log transitionLog("Node "+std::to_string(nodeId)+" is in transmission Mode", true);
        // logger.logMessage(transitionLog);  
        if(beaconSlots.size()==0 && !shouldSendBeacon){
            return true;//we finished to send our beacons, but the node stays in transmission mode to not fuck up my implementation lol, would be possible to implement this behaviour but it's just simpler like this
                        //overall this simulator is far from being close to a real behavior of a node
        }
        else if(beaconSlots.size()==0 &&shouldSendBeacon){// we compute the randomness of slots (see the thesis report)
            shouldSendBeacon=false;
            beaconSlots=selectRandomSlots(computeRandomNbBeaconPackets());
        }

        if(beaconSlots[0]==0){
            //send the beacon
            std::vector<uint8_t> beaconPacket = {0x01, 0x02, 0x03, 0x04};
            std::this_thread::sleep_for(std::chrono::milliseconds(guardTime)); 
            addMessageToTransmit(beaconPacket,std::chrono::milliseconds(timeOnAirBeacon));
            //erase the first element of the list
            beaconSlots.erase(beaconSlots.begin());
        }
        //decrease every elements of the slots by one
        for(int i=0;i<beaconSlots.size();i++){
            beaconSlots[i]--;
        }
        return true;
    }

    bool C3_Node::canSleepFromTransmitting() { 
        currentState=NodeState::Sleeping;
        // Log transitionLog("Node "+std::to_string(nodeId)+" sleeps", true);
        // logger.logMessage(transitionLog);  
        return true; 
        }


    //Unauthorized transition in this mode.
    bool C3_Node::canTransmitFromTransmitting() { return false; }
    bool C3_Node::canTransmitFromCommunicating(){return false;}
    bool C3_Node::canTransmitFromListening() { return false; }
    bool C3_Node::canListenFromTransmitting() { return false; }
    bool C3_Node::canListenFromSleeping() { return false; }
    bool C3_Node::canListenFromListening() { return false; }
    bool C3_Node::canListenFromCommunicating(){return true;}
    bool C3_Node::canSleepFromListening() { return false; }
    bool C3_Node::canSleepFromSleeping() { return false; }
    bool C3_Node::canSleepFromCommunicating(){return false;}
    bool C3_Node::canCommunicateFromTransmitting(){return false;}
    bool C3_Node::canCommunicateFromListening(){return false;}
    bool C3_Node::canCommunicateFromSleeping(){return false;}
    bool C3_Node::canCommunicateFromCommunicating(){return false;}


#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif

