#include "C2_Node.hpp"
#include "../Common.hpp"


    std::string C2_Node::initMessage() const{

        std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

        return finalMsg;
    }


#if COMMUNICATION_PERIOD == RRC_BEACON

    bool C2_Node::canNodeReceiveMessage() {
        // State Condition: node must be listening to receive a message
        if(currentState!=NodeState::Listening&&currentState!=NodeState::Communicating){
            return false;
        }
        else if(currentState==NodeState::Communicating){
                return !isTransmittingWhileCommunicating;
        }
        return true;
    }

    void C2_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir)
    {
        if(!canNodeReceiveMessage()){
            Log interferenceLog("Node "+std::to_string(nodeId)+" is not listening, message "+packet_to_binary(message)+" is lost", true);
            logger.logMessage(interferenceLog);
            return;
        }


        Node::receiveMessage(message, timeOnAir);//for the interference model
        //there is one message in the receiveBuffer now

        std::lock_guard<std::mutex> lock(receiveMutex);

        //TODO: add logic to see if the new packet is worth to be processed

        shouldSendBeacon=true;

        Log receivedLog("Node "+std::to_string(nodeId)+" received "+packet_to_binary(message), true);
        logger.logMessage(receivedLog);
    
    }

    int C2_Node::computeRandomNbBeaconPackets()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(minimumNbBeaconPackets, maximumNbBeaconPackets);
        return dis(gen);
    }

    std::vector<int> C2_Node::selectRandomSlots(int m)
    {
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

    bool C2_Node::canSleepFromCommunicating()
    {   //Node Can alwasy sleep
        currentState=NodeState::Sleeping;
        Log transitionLog("Node "+std::to_string(nodeId)+" sleeps", true);
        logger.logMessage(transitionLog);   
        return true;
    }

    bool C2_Node::canCommunicateFromSleeping() { 
        isTransmittingWhileCommunicating=false;

        currentState=NodeState::Communicating;
        if(shouldSendBeacon&&beaconSlots.size()==0){
            //a "new" beacon has just been received, we plan the random slots
            shouldSendBeacon=false;
            beaconSlots=selectRandomSlots(computeRandomNbBeaconPackets());
        }
        if(beaconSlots.size()>0){
            //we have beacons to send 
            if(beaconSlots[0]==0){
                isTransmittingWhileCommunicating=true;
                //send the beacon
                std::vector<uint8_t> beaconPacket = {0x01, 0x02, 0x03, 0x04};
                std::this_thread::sleep_for(std::chrono::milliseconds(guardTime));
                addMessageToTransmit(beaconPacket,std::chrono::milliseconds(timeOnAirBeacon));
                beaconSlots.erase(beaconSlots.begin());
            }
            //decrease every elements of the slots by one
            for(int i=0;i<beaconSlots.size();i++){
                beaconSlots[i]--;
            }
        }             
        return true; 
    }



    //Unauthorized transition in this mode.
    bool C2_Node::canCommunicateFromTransmitting() { return false; }
    bool C2_Node::canCommunicateFromListening() { return false; }
    bool C2_Node::canCommunicateFromCommunicating() { return true; }
    bool C2_Node::canTransmitFromListening() { return true; }
    bool C2_Node::canTransmitFromSleeping() {return true; }
    bool C2_Node::canTransmitFromTransmitting() { return true; }
    bool C2_Node::canTransmitFromCommunicating(){return false;}
    bool C2_Node::canListenFromTransmitting() { return true; }
    bool C2_Node::canListenFromSleeping() {return false;}
    bool C2_Node::canListenFromListening() { return false; }
    bool C2_Node::canListenFromCommunicating(){return false;}
    bool C2_Node::canSleepFromTransmitting() { return false; }
    bool C2_Node::canSleepFromListening() { return false; }
    bool C2_Node::canSleepFromSleeping() { return false; }


#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif