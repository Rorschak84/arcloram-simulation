#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}

    bool C3_Node::canIdleFromTransmitting() { 
            Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Idling", true);
            logger.logMessage(transitionLog);     
        return true; }
    bool C3_Node::canIdleFromListening() { return true; }
    bool C3_Node::canIdleFromSleeping() { return true; }
    bool C3_Node::canIdleFromIdling() { return true; }

    bool C3_Node::canTransmitFromIdling() { return true; }
    bool C3_Node::canTransmitFromListening() { return true; }
    bool C3_Node::canTransmitFromSleeping() { 
        Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Transmitting", true);
        logger.logMessage(transitionLog);     
        // Sleep for 30 ms to be sure other nodes are listening, this offset represents the 
        //beginning of the transmission window in the communication window+the guard time.
        //I'm afraid this guard time also depends of the number of nodes performing tasks at the same time... (small impact though)
        //TODO: try to use steady_clock instead of system_clock when you can to migitate the impact of the system on the time
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
        addMessageToTransmit("Hello from Node: "+std::to_string(nodeId),std::chrono::milliseconds(500));
        return true;
        }
    bool C3_Node::canTransmitFromTransmitting() { return true; }

    bool C3_Node::canListenFromIdling() { return true; }
    bool C3_Node::canListenFromTransmitting() { return true; }
    bool C3_Node::canListenFromSleeping() { return true; }
    bool C3_Node::canListenFromListening() { return true; }

    bool C3_Node::canSleepFromIdling() { return true; }
    bool C3_Node::canSleepFromTransmitting() { return true; }
    bool C3_Node::canSleepFromListening() { return true; }
    bool C3_Node::canSleepFromSleeping() { return true; }
