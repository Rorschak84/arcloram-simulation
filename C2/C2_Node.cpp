#include "C2_Node.hpp"
#include "../Common.hpp"
#include "../TCP/packets.hpp"	


    std::string C2_Node::initMessage() const{

        std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";
    
    sf::Packet positionPacketReceiver;
    positionPacket positionPacket(nodeId,2,coordinates);
    positionPacketReceiver<<positionPacket;
    logger.sendTcpPacket(positionPacketReceiver);
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

    void C2_Node::addPacket(SenderID sender, PacketID packet)
    {
        
        packetsMap[sender].push_back(packet);

    }

    bool C2_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir)
    {
        if(!canNodeReceiveMessage()){
             Log notlisteninglog("Node "+std::to_string(nodeId)+" not listening, dropped msg"/*+detailedBytesToString( message)*/, true);
             logger.logMessage(notlisteninglog);

            sf::Packet receptionStatePacketReceiver;
            uint16_t senderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
            receiveMessagePacket receptionState(senderId,nodeId,"notListening");
            receptionStatePacketReceiver<<receptionState;
            logger.sendTcpPacket(receptionStatePacketReceiver);

            return false;
        }

       if(!Node::receiveMessage(message, timeOnAir)){
            //an interference happened, we don't treat the message
            
            sf::Packet receptionStatePacketReceiver;
            uint16_t senderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
            receiveMessagePacket receptionState(senderId,nodeId,"interference");
            receptionStatePacketReceiver<<receptionState;
            logger.sendTcpPacket(receptionStatePacketReceiver);

            return false;
       } 


            sf::Packet receptionStatePacketReceiver;
            uint16_t senderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
            receiveMessagePacket receptionState(senderId,nodeId,"received");
            receptionStatePacketReceiver<<receptionState;
            logger.sendTcpPacket(receptionStatePacketReceiver);

        uint8_t type=extractBytesFromField(message,"type",common::fieldMap);
        if(type!=common::type[0]){
            //not a beacon, we don't care
            Log wrongTypeLog("Node "+std::to_string(nodeId)+" received Incorrecty packet type, dropping", true);
            logger.logMessage(wrongTypeLog);
            // receiveBuffer.pop();
            return false;
        }


        uint8_t packetHopCount=extractBytesFromField(message,"hopCount",common::fieldMap);
        uint32_t packetTimeStamp=extractBytesFromField(message,"timeStamp",common::fieldMap);
        uint16_t packetGlobalIDPacket=extractBytesFromField(message,"globalIDPacket",common::fieldMap);
        uint8_t packetPathCost=extractBytesFromField(message,"costFunction",common::fieldMap);
        uint16_t packetNextNodeIdInPath=extractBytesFromField(message,"senderGlobalId",common::fieldMap);

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

            sf::Packet routingPacketReceiver;
            routingDecisionPacket routingPacket(nodeId,nextNodeIdInPath.value(),true);
            routingPacketReceiver<<routingPacket;
            logger.sendTcpPacket(routingPacketReceiver);

            Log rootingLog("Node "+std::to_string(nodeId)+" rooting with Node:"+std::to_string(nextNodeIdInPath.value()), true);
            logger.logMessage(rootingLog);

            return true;
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

                    //we supress the old routing in the visualiser....
                    sf::Packet routingPacketReceiver;
                    routingDecisionPacket routingPacket(nodeId,nextNodeIdInPath.value(),false);
                    routingPacketReceiver<<routingPacket;
                    logger.sendTcpPacket(routingPacketReceiver);

                    Log oldRootingLog("Node "+std::to_string(nodeId)+" FORGETTING rooting with Node:"+std::to_string(nextNodeIdInPath.value()), true);
                    logger.logMessage(oldRootingLog);

                    nextNodeIdInPath=  packetNextNodeIdInPath; 

                    Log rootingLog("Node "+std::to_string(nodeId)+" rooting with Node:"+std::to_string(nextNodeIdInPath.value()), true);
                    logger.logMessage(rootingLog);

                    //and add the new one
                    sf::Packet routingPacketReceiver2;
                    routingDecisionPacket routingPacket2(nodeId,nextNodeIdInPath.value(),true);
                    routingPacketReceiver2<<routingPacket2;
                    logger.sendTcpPacket(routingPacketReceiver2); 
                }
            }

   
            //TODO: put the +4 in common
            if (hopCount.value() +4>packetHopCount){
                // the timestamp received can be included in the synchronization clock mechanism as it has a similar relative accuracy
                
                lastTimeStampReceived=packetTimeStamp;//there should be a function to actualize the internal clock here

                if(std::find(globalIDPacketList.begin(), globalIDPacketList.end(), packetGlobalIDPacket) != globalIDPacketList.end()){
                    Log alreadyBeacon("Node "+std::to_string(nodeId)+" already received this beacon, dropping", true);
                    logger.logMessage(alreadyBeacon);
                    
                }else{
                    //it's a new beacon, we reenter boradcast mode regardless of the beacons left to send
                    globalIDPacketList.push_back(packetGlobalIDPacket);
                    shouldSendBeacon=true;//at next transmission slots, will create the new sending beacon scheduler
                    beaconSlots.clear();
                }
            }    

            return true;   //it's for the compiler to not throw a warning, we never capture this variable                     
        }
    }
    
    //----------------------------STATE TRANSITIONS--------------------------------

    bool C2_Node::canSleepFromCommunicating()
    {   //Node Can alwasy sleep
        sf::Packet statePacketReceiver;
        stateNodePacket statePacket(nodeId, "Sleep");
        statePacketReceiver<<statePacket;
        logger.sendTcpPacket(statePacketReceiver);

        currentState=NodeState::Sleeping;
        // Log transitionLog("Node "+std::to_string(nodeId)+" sleeps", true);
        // logger.logMessage(transitionLog);   
        return true;
    }

    bool C2_Node::canCommunicateFromSleeping() { 

        sf::Packet statePacketReceiver;
        stateNodePacket statePacket(nodeId, "Communicate");
        statePacketReceiver<<statePacket;
        logger.sendTcpPacket(statePacketReceiver);

        isTransmittingWhileCommunicating=false;
        currentState=NodeState::Communicating;
        if(shouldSendBeacon&&beaconSlots.size()==0){
            //a "new" beacon has just been received, we plan the random slots
            shouldSendBeacon=false;
            beaconSlots=selectRandomSlots(computeRandomNbBeaconPackets(common::minimumNbBeaconPackets,common::maximumNbBeaconPackets),common::nbSlotsPossibleForOneBeacon);
            std::ostringstream oss;
            for (size_t i = 0; i < beaconSlots.size(); ++i) {
                oss << beaconSlots[i];
                if (i < beaconSlots.size() - 1) {
                    oss << ", "; // Add a separator between elements
                 }

            } 
            // Log beaconSlotsLog("Node "+std::to_string(nodeId)+" will send beacons at slots: "+oss.str(), true);
            // logger.logMessage(beaconSlotsLog);
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

                sf::Packet broadcastPacketReceiver;
                broadcastMessagePacket broadcastPacket(nodeId);
                broadcastPacketReceiver<<broadcastPacket;
                logger.sendTcpPacket(broadcastPacketReceiver);

                addMessageToTransmit(beaconPacket,std::chrono::milliseconds(common::timeOnAirBeacon));
                beaconSlots.erase(beaconSlots.begin());
            }
            if(!beaconSlots.empty()){
                 //decrease every elements of the slots by one
                for(int i=0;i<beaconSlots.size();i++){
                    beaconSlots[i]--;
                }
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


#elif COMMUNICATION_PERIOD == RRC_DOWNLINK

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


bool C2_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir)
    {
        if(!canNodeReceiveMessage()){
             Log notlisteninglog("Node "+std::to_string(nodeId)+" not listening, dropped msg"/*+detailedBytesToString( message)*/, true);
             logger.logMessage(notlisteninglog);


             sf::Packet receptionStatePacketReceiver;
            uint16_t senderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
            receiveMessagePacket receptionState(senderId,nodeId,"notListening");
            receptionStatePacketReceiver<<receptionState;
            logger.sendTcpPacket(receptionStatePacketReceiver);

            return false;
        }

       if(!Node::receiveMessage(message, timeOnAir)){
            //an interference happened, we don't treat the message
            sf::Packet receptionStatePacketReceiver;
            uint16_t senderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
            receiveMessagePacket receptionState(senderId,nodeId,"interference");
            receptionStatePacketReceiver<<receptionState;
            logger.sendTcpPacket(receptionStatePacketReceiver);



            return false;
       } 



        uint8_t type=extractBytesFromField(message,"type",common::fieldMap);
        if(type!=common::type[0]){
            //not a beacon, we don't care
            Log wrongTypeLog("Node "+std::to_string(nodeId)+" received Incorrecty packet type, dropping", true);
            logger.logMessage(wrongTypeLog);
            // receiveBuffer.pop();
            return false;
        }

        sf::Packet receptionStatePacketReceiver;
            uint16_t senderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
            receiveMessagePacket receptionState(senderId,nodeId,"received");
            receptionStatePacketReceiver<<receptionState;
            logger.sendTcpPacket(receptionStatePacketReceiver);

        uint8_t packetSenderId=extractBytesFromField(message,"senderGlobalId",common::fieldMap);
        uint32_t packetReceiverId=extractBytesFromField(message,"receiverGlobalId",common::fieldMap);
        uint16_t packetGlobalIDPacket=extractBytesFromField(message,"globalIDPacket",common::fieldMap);
        uint8_t packetPayload=extractBytesFromField(message,"payload",common::fieldMap);
        uint8_t packetHashFunction=extractBytesFromField(message,"hashFunction",common::fieldMap);
        // //TODO: remove the buffer?? maybe it's just used by the simulation manager
        // receiveBuffer.pop();//we don't care about the other messages
        // //actually, we don't really care about the receiving buffer, since each message is treated as soon as it is received.

        if(packetReceiverId==nodeId){//add C1 Childs condition when you implement more complex topology
            //THe packet is for us!
            Log finalReceiverLog("Node "+std::to_string(nodeId)+" received a packet for him", true);
            logger.logMessage(finalReceiverLog);
            //TODO: make clear in the visualiser that there is something hapenning?
            //woud be nice you reuse what you implemented, and don't make another packet

            //maybe put something to signify in the visualiser that It Works TODO
            return true;
        }

        std::lock_guard<std::mutex> lock(receiveMutex);
        if(globalIDPacketList.empty()){
            //this is the first beacon received
            shouldSendBeacon=true; //next tranmission slots, will create the new sending beacon scheduler
           globalIDPacketList.push_back(packetGlobalIDPacket);     
           packetFinalReceiverId=packetReceiverId;   
            return true;
        }
        else{ 
   
                if(std::find(globalIDPacketList.begin(), globalIDPacketList.end(), packetGlobalIDPacket) != globalIDPacketList.end()){
                    Log alreadyBeacon("Node "+std::to_string(nodeId)+" already received this beacon, dropping", true);
                    logger.logMessage(alreadyBeacon);
                    
                }else{
                    //This case should not happen too often, the C1 are supposed to wait 
                    globalIDPacketList.push_back(packetGlobalIDPacket);
                    shouldSendBeacon=true;//at next transmission slots, will create the new sending beacon scheduler
                    beaconSlots.clear();
                }
            }    

            return true;   //it's for the compiler to not throw a warning, we never capture this variable                     
        }
    

    //----------------------------STATE TRANSITIONS--------------------------------

   bool C2_Node::canCommunicateFromSleeping() { 

        sf::Packet statePacketReceiver;
        stateNodePacket statePacket(nodeId, "Communicate");
        statePacketReceiver<<statePacket;
        logger.sendTcpPacket(statePacketReceiver);

        isTransmittingWhileCommunicating=false;
        currentState=NodeState::Communicating;
        if(shouldSendBeacon&&beaconSlots.size()==0){
            //a "new" beacon has just been received, we plan the random slots
            shouldSendBeacon=false;
            beaconSlots=selectRandomSlots(computeRandomNbBeaconPackets(common::minimumNbBeaconPackets,common::maximumNbBeaconPackets),common::nbSlotsPossibleForOneBeacon);
            std::ostringstream oss;
            for (size_t i = 0; i < beaconSlots.size(); ++i) {
                oss << beaconSlots[i];
                if (i < beaconSlots.size() - 1) {
                    oss << ", "; // Add a separator between elements
                 }

            } 
            // Log beaconSlotsLog("Node "+std::to_string(nodeId)+" will send beacons at slots: "+oss.str(), true);
            // logger.logMessage(beaconSlotsLog);
        }
        if(beaconSlots.size()>0){
            //we have beacons to send 
            if(beaconSlots[0]==0){
                isTransmittingWhileCommunicating=true;
                std::this_thread::sleep_for(std::chrono::milliseconds(common::guardTime));


            //-------------------------------------define Beacon Packet----------------------------
            //----------least signficant byte first (little endian) !------------
            std::vector<uint8_t> globalIDPacket= decimalToBytes( globalIDPacketList.back(), common::globalIDPacketBytesSize); //Global ID is 2 byte long in the simulation, 10 bits in real life
            std::vector<uint8_t> senderGlobalId = decimalToBytes(nodeId,2); //Sender Global ID is 2 byte long in the simulation, 10 bits in real life
           
            std::vector<uint8_t> receiverGlobalId = decimalToBytes(packetFinalReceiverId.value(),2); //Sender Global ID is 2 byte long in the simulation, 10 bits in real life
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

            sf::Packet broadcastPacketReceiver;
            broadcastMessagePacket broadcastPacket(nodeId);
            broadcastPacketReceiver<<broadcastPacket;
            logger.sendTcpPacket(broadcastPacketReceiver);

                addMessageToTransmit(beaconPacket,std::chrono::milliseconds(common::timeOnAirFlood));
                beaconSlots.erase(beaconSlots.begin());
            }
            if(!beaconSlots.empty()){
                 //decrease every elements of the slots by one
                for(int i=0;i<beaconSlots.size();i++){
                    beaconSlots[i]--;
                }
            }
           
        }             
        return true; 
    }

    bool C2_Node::canSleepFromCommunicating()
    {   //Node Can alwasy sleep

        sf::Packet statePacketReceiver;
        stateNodePacket statePacket(nodeId, "Sleep");
        statePacketReceiver<<statePacket;
        logger.sendTcpPacket(statePacketReceiver);

        currentState=NodeState::Sleeping;
        // Log transitionLog("Node "+std::to_string(nodeId)+" sleeps", true);
        // logger.logMessage(transitionLog);   
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

#elif COMMUNICATION_PERIOD == RRC_UPLINK

bool C2_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir){
    return true;
}


    bool C2_Node::canCommunicateFromSleeping(){

        currentState=NodeState::Communicating;

        sf::Packet statePacketReceiver;
        stateNodePacket statePacket(nodeId, "Communicate");
        statePacketReceiver<<statePacket;
        logger.sendTcpPacket(statePacketReceiver);

        //is it a ACK window or a data window?
        if(!isDataWindow){
            isDataWindow=true;//we switch to the data window
            
        }

        return true;
    }
    bool C2_Node::canSleepFromCommunicating(){

        currentState=NodeState::Sleeping;
        sf::Packet statePacketReceiver;
        stateNodePacket statePacket(nodeId, "Sleep");
        statePacketReceiver<<statePacket;
        logger.sendTcpPacket(statePacketReceiver);
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