#include "C2_Node.hpp"
#include "../Common.hpp"


    std::string C2_Node::initMessage() const{

        std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

        return finalMsg;
    }


#if COMMUNICATION_PERIOD == RRC_BEACON

    // void C2_Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir)
    // {
    //     Node::receiveMessage(message, timeOnAir);

    //     Log receivedLog("Node "+std::to_string(nodeId)+" received "+packet_to_binary(message), true);
    //     logger.logMessage(receivedLog);
    // }

    bool C2_Node::canSleepFromCommunicating()
    {   //Node Can alwasy sleep
        Log transitionLog("Node "+std::to_string(nodeId)+" sleeps", true);
        logger.logMessage(transitionLog);   
        return true;
    }

    bool C2_Node::canCommunicateFromSleeping() { 
        
        //pseudo-code:
        //if beacon bool (meaning we have a new beacon to send, variables are already changed)
        //more or less the same as in C3
        Log transitionLog("Node "+std::to_string(nodeId)+" is in communication Mode", true);
        logger.logMessage(transitionLog);   
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