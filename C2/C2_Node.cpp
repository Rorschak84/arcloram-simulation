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
             Log notlisteninglog("Node "+std::to_string(nodeId)+" not listening, dropped "+detailedBytesToString( message), true);
             logger.logMessage(notlisteninglog);
            return;
        }

        Node::receiveMessage(message, timeOnAir);//for the interference model
        //there is one message in the receiveBuffer no
        uint8_t type=extractBytesFromField(message,"type");
        if(type!=common::type[0]){
            //not a beacon, we don't care
            Log wrongTypeLog("Node "+std::to_string(nodeId)+" received Incorrecty packet type, dropping", true);
            logger.logMessage(wrongTypeLog);
            receiveBuffer.pop();
            return;
        }

        shouldSendBeacon=true;

        uint8_t packetHopCount=extractBytesFromField(message,"hopCount");
        uint32_t packetTimeStamp=extractBytesFromField(message,"timeStamp");
        uint16_t packetGlobalIDPacket=extractBytesFromField(message,"globalIDPacket");
        uint8_t packetPathCost=extractBytesFromField(message,"costFunction");
        uint16_t packetNextNodeIdInPath=extractBytesFromField(message,"senderGlobalId");

        // //TODO: remove the buffer?? maybe it's just used by the simulation manager
        // receiveBuffer.pop();//we don't care about the other messages
        // //actually, we don't really care about the receiving buffer, since each message is treated as soon as it is received.

        std::lock_guard<std::mutex> lock(receiveMutex);
        if(!hopCount.has_value()){
            //this is the first beacon received
            shouldSendBeacon=true; //next tranmission slots, will create the new sending beacon scheduler
            hopCount=packetHopCount+1;
            lastTimeStampReceived=packetTimeStamp;
            globalIDPacketList.push_back(packetGlobalIDPacket);
            pathCost=packetPathCost+5; //should take into account the battery TODO
            nextNodeIdInPath=packetNextNodeIdInPath;
        }
        else{ 
            if(nextNodeIdInPath==packetNextNodeIdInPath){
                //we received a Beacon from the optimized path, but we need to check if the associated cost changed
                if(pathCost<packetPathCost){
                    //the cost has changed
                    pathCost=packetPathCost+5;
                    hopCount=packetHopCount+1;//it can happen that the next Optimal Node in the path found a new optimal path itself, thus changing the hop count
                }
            }
            else{
            //check if the registerd path is still the least costly, otherwise update the path
                if(pathCost>packetPathCost){
                     //the optimized path has changed - the path is independent from the fact we resend beacon
                    pathCost=packetPathCost+5;
                    hopCount=packetHopCount+1;
                    nextNodeIdInPath=  packetNextNodeIdInPath;     
                }
            }

   
            //TODO: put the +4 in common
            if (hopCount.value() +4>packetHopCount){
                // the timestamp received can be included in the synchronization clock mechanism as it has a similar relative accuracy
                
                lastTimeStampReceived=packetTimeStamp;//there should be a function to actualize the internal clock here

                if(std::find(globalIDPacketList.begin(), globalIDPacketList.end(), packetGlobalIDPacket) != globalIDPacketList.end()){
                    //we already received this beacon, no retransmission
                    return;
                }else{
                    //it's a new beacon, we reenter boradcast mode regardless of the beacons left to send
                    globalIDPacketList.push_back(packetGlobalIDPacket);
                    shouldSendBeacon=true;//at next transmission slots, will create the new sending beacon scheduler
                    beaconSlots.clear();
                }
            }                               
        }
    }
    

    // TODO: put them in the common
    int C2_Node::computeRandomNbBeaconPackets()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(common::minimumNbBeaconPackets,common::maximumNbBeaconPackets);
        return dis(gen);
    }

    std::vector<int> C2_Node::selectRandomSlots(int m)
    {
        // Step 1: Create a vector of slots [0, 1, ..., n-1]
        std::vector<int> slots(common::nbSlotsPossibleForOneBeacon);
        for (int i = 0; i < common::nbSlotsPossibleForOneBeacon; ++i) {
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
        // Log transitionLog("Node "+std::to_string(nodeId)+" sleeps", true);
        // logger.logMessage(transitionLog);   
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
                std::this_thread::sleep_for(std::chrono::milliseconds(common::guardTime));

                //create the beacon packet
                std::vector<uint8_t> beaconPacket;
                //preallocate the space for optimization
                beaconPacket.reserve(common::typeBytesSize + common::timeStampBytesSize + common::costFunctionBytesSize +
                                common::hopCountBytesSize + common::globalIDPacketBytesSize +
                                common::senderGlobalIdBytesSize + common::hashFunctionBytesSize);
                
                //prepare the fields
                std::vector<uint8_t> newTimeStamp = getTimeStamp(); 
                std::vector<uint8_t> newCostFunction = {pathCost.value()};
                std::vector<uint8_t> newHopCount = decimalToBytes(hopCount.value(), common::hopCountBytesSize);
                std::vector<uint8_t> newGlobalIDPacket = decimalToBytes( globalIDPacketList.back(), common::globalIDPacketBytesSize);
                std::vector<uint8_t> newSenderGlobalId = decimalToBytes(nodeId, common::senderGlobalIdBytesSize);
                std::vector<uint8_t> newHashFunction = {0x00,0x00,0x00,0x00}; 

                // Append all fields
                appendVector(beaconPacket, common::type);
                appendVector(beaconPacket, newTimeStamp);
                appendVector(beaconPacket, newCostFunction);
                appendVector(beaconPacket, newHopCount);
                appendVector(beaconPacket, newGlobalIDPacket);
                appendVector(beaconPacket, newSenderGlobalId);
                appendVector(beaconPacket, newHashFunction);

                addMessageToTransmit(beaconPacket,std::chrono::milliseconds(common::timeOnAirBeacon));
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