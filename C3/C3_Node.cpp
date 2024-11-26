#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}


    bool C3_Node::canTransmitFromListening() { return true; }
    bool C3_Node::canTransmitFromSleeping() { 




        



        Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to Transmitting", true);
        logger.logMessage(transitionLog);     
        // Sleep for 30 ms to be sure other nodes are listening, this offset represents the 
        //beginning of the transmission window in the communication window+the guard time.
        //I'm afraid this guard time also depends of the number of nodes performing tasks at the same time... (small impact though)
        //TODO: try to use steady_clock instead of system_clock when you can to migitate the impact of the system on the time
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
        std::vector<uint8_t> packet = {0x01, 0x02, 0x03, 0x04};
        addMessageToTransmit(packet,std::chrono::milliseconds(500));
        return true;
        }


    bool C3_Node::canTransmitFromTransmitting() { return true; }

    bool C3_Node::canTransmitFromCommunicating()
    {
        return false;
    }

    bool C3_Node::canListenFromTransmitting() { return true; }
    bool C3_Node::canListenFromSleeping() { return true; }
    bool C3_Node::canListenFromListening() { return true; }
    bool C3_Node::canListenFromCommunicating()
    {
        return true;
    }

    bool C3_Node::canSleepFromTransmitting() { return true; }
    bool C3_Node::canSleepFromListening() { return true; }
    bool C3_Node::canSleepFromSleeping() { return true; }

    bool C3_Node::canSleepFromCommunicating()
    {
        return true;
    }

    bool C3_Node::canCommunicateFromTransmitting()
    {
        return true;
    }

    bool C3_Node::canCommunicateFromListening()
    {
        return true;
    }

    bool C3_Node::canCommunicateFromSleeping()
    {
        return true;
    }

    bool C3_Node::canCommunicateFromCommunicating()
    {
        return true;
    }
