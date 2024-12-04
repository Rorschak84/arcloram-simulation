#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}



#if COMMUNICATION_PERIOD == RRC_BEACON


    bool C3_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir){
            Log notlisteninglog("Node "+std::to_string(nodeId)+" not listening, dropped msg "/*+detailedBytesToString( message)*/, true);
             logger.logMessage(notlisteninglog);
            return false;
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


            std::this_thread::sleep_for(std::chrono::milliseconds(common::guardTime)); 



            //-------------------------------------define Beacon Packet----------------------------
            //----------least signficant byte first (little endian) !------------
                      
            std::vector<uint8_t> timeStamp = getTimeStamp(); //look how to provision it Timestamp is 4 bytes long in the simulation AND real life
            std::vector<uint8_t> costFunction = {0x00}; //Cost Function is 1 byte long in the simulation, 12 bits in real life
            std::vector<uint8_t> hopCount = {0x00,0x00}; //Hop Count is 2 byte long in the simulation, 10 bits in real life

            std::vector<uint8_t> globalIDPacket= {0x01,0x00 }; //Global ID is 2 byte long in the simulation, 10 bits in real life
            std::vector<uint8_t> senderGlobalId = decimalToBytes(nodeId,2); //Sender Global ID is 2 byte long in the simulation, 10 bits in real life
            //dummy hash: we don't implement the hash function in this simulation
            std::vector<uint8_t> hashFunction = {0x00,0x00,0x00,0x00}; //Hash Function is 4 byte long in the simulation AND in real life



            // Concatenate fields into one vector
            std::vector<uint8_t> beaconPacket;

            //preallocate the space for optimization
            //TODO: should use the size in the common file, not the variable, source of error
            beaconPacket.reserve(common::type.size() + timeStamp.size() + costFunction.size() +
                            hopCount.size() + globalIDPacket.size() +
                            senderGlobalId.size() + hashFunction.size());

            // Append all fields
            appendVector(beaconPacket, common::type);
            appendVector(beaconPacket, timeStamp);
            appendVector(beaconPacket, costFunction);
            appendVector(beaconPacket, hopCount);
            appendVector(beaconPacket, globalIDPacket);
            appendVector(beaconPacket, senderGlobalId);
            appendVector(beaconPacket, hashFunction);



            addMessageToTransmit(beaconPacket,std::chrono::milliseconds(common::timeOnAirBeacon));
            //erase the first element of the list
            beaconSlots.erase(beaconSlots.begin());
        }
        //decrease every elements of the slots by one
        if(beaconSlots.size()==0){
            return true;
        }
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

#elif COMMUNICATION_PERIOD == RRC_DOWNLINK

   bool C3_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir){
            Log notlisteninglog("Node "+std::to_string(nodeId)+" not listening, dropped msg "/*+detailedBytesToString( message)*/, true);
             logger.logMessage(notlisteninglog);
            return false;
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


            std::this_thread::sleep_for(std::chrono::milliseconds(common::guardTime)); 



            //-------------------------------------define Beacon Packet----------------------------
            //----------least signficant byte first (little endian) !------------
            std::vector<uint8_t> globalIDPacket= {0x01,0x00 }; //Global ID is 2 byte long in the simulation, 10 bits in real life
            std::vector<uint8_t> senderGlobalId = decimalToBytes(nodeId,2); //Sender Global ID is 2 byte long in the simulation, 10 bits in real life
           
            std::vector<uint8_t> receiverGlobalId = decimalToBytes(3,2); //Sender Global ID is 2 byte long in the simulation, 10 bits in real life
            std::vector<uint8_t> payload = {0xFF,0xFF,0xFF,0xFF}; //Payload Size is 4 byte long in the simulation, 40 Bytes max in real life
           
            //dummy hash: we don't implement the hash function in this simulation
            std::vector<uint8_t> hashFunction = {0x00,0x00,0x00,0x00}; //Hash Function is 4 byte long in the simulation AND in real life



            // Concatenate fields into one vector
            std::vector<uint8_t> beaconPacket;

            //preallocate the space for optimization
            //TODO: should use the size in the common file, not the variable, source of error
            beaconPacket.reserve(common::type.size() + 
                            senderGlobalId.size() +
                            receiverGlobalId.size() +
                            globalIDPacket.size() +
                            payload.size() +                            
                            + hashFunction.size());

            // Append all fields
            appendVector(beaconPacket, common::type);
            appendVector(beaconPacket, senderGlobalId);
            appendVector(beaconPacket, receiverGlobalId);
            appendVector(beaconPacket, globalIDPacket);
            appendVector(beaconPacket, payload);
            appendVector(beaconPacket, hashFunction);



            addMessageToTransmit(beaconPacket,std::chrono::milliseconds(common::timeOnAirFlood));
            //erase the first element of the list
            beaconSlots.erase(beaconSlots.begin());
        }
        //decrease every elements of the slots by one
        if(beaconSlots.size()==0){
            return true;
        }
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

