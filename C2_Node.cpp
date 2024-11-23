#include "C2_Node.hpp"

std::string C2_Node::initMessage() const{

    std::string msg=Node::initMessage();
   std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}



    bool C2_Node::canIdleFromTransmitting() { return true; }
    bool C2_Node::canIdleFromListening() { 
            Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Idling", true);
            logger.logMessage(transitionLog);     
        return true; }
    bool C2_Node::canIdleFromSleeping() { return true; }
    bool C2_Node::canIdleFromIdling() { return true; }

    bool C2_Node::canTransmitFromIdling() { return true; }
    bool C2_Node::canTransmitFromListening() { return true; }
    bool C2_Node::canTransmitFromSleeping() { 
        
        Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to transmitting", true);
        logger.logMessage(transitionLog);     
        
        return true; 
        }
    bool C2_Node::canTransmitFromTransmitting() { return true; }

    bool C2_Node::canListenFromIdling() { return true; }
    bool C2_Node::canListenFromTransmitting() { return true; }
    bool C2_Node::canListenFromSleeping() { 

            Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Listening", true);
            logger.logMessage(transitionLog);     
        
        return true; 
        }
    bool C2_Node::canListenFromListening() { return true; }

    bool C2_Node::canSleepFromIdling() { return true; }
    bool C2_Node::canSleepFromTransmitting() { return true; }
    bool C2_Node::canSleepFromListening() { return true; }
    bool C2_Node::canSleepFromSleeping() { return true; }
