#include "C2_Node.hpp"
#include "../Common.hpp"


std::string C2_Node::initMessage() const{

    std::string msg=Node::initMessage();
   std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}



    bool C2_Node::canCommunicateFromTransmitting() { return true; }
    bool C2_Node::canCommunicateFromListening() { 
            Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Communicating", true);
            logger.logMessage(transitionLog);     
        return true; }
    bool C2_Node::canCommunicateFromSleeping() { return true; }
    bool C2_Node::canCommunicateFromCommunicating() { return true; }

    bool C2_Node::canTransmitFromListening() { return true; }
    bool C2_Node::canTransmitFromSleeping() { 
        
        Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to transmitting", true);
        logger.logMessage(transitionLog);     
        
        return true; 
        }
    bool C2_Node::canTransmitFromTransmitting() { return true; }

    bool C2_Node::canTransmitFromCommunicating()
    {
        return false;
    }

    bool C2_Node::canListenFromTransmitting() { return true; }
    bool C2_Node::canListenFromSleeping() { 

            Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Listening", true);
            logger.logMessage(transitionLog);     
        
        return true; 
        }
    bool C2_Node::canListenFromListening() { return true; }

    bool C2_Node::canListenFromCommunicating()
    {
        return false;
    }

    bool C2_Node::canSleepFromTransmitting() { return true; }
    bool C2_Node::canSleepFromListening() { return true; }
    bool C2_Node::canSleepFromSleeping() { return true; }

    bool C2_Node::canSleepFromCommunicating()
    {
        return false;
    }
