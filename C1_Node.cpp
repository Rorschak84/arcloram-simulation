#include "C1_Node.hpp"


std::string C1_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}

    bool C1_Node::canIdleFromTransmitting() { return true; }
    bool C1_Node::canIdleFromListening() { return true; }
    bool C1_Node::canIdleFromSleeping() { return true; }
    bool C1_Node::canIdleFromIdling() { return true; }

    bool C1_Node::canTransmitFromIdling() { return true; }
    bool C1_Node::canTransmitFromListening() { return true; }
    bool C1_Node::canTransmitFromSleeping() { return true; }
    bool C1_Node::canTransmitFromTransmitting() { return true; }

    bool C1_Node::canListenFromIdling() { return true; }
    bool C1_Node::canListenFromTransmitting() { return true; }
    bool C1_Node::canListenFromSleeping() { return true; }
    bool C1_Node::canListenFromListening() { return true; }

    bool C1_Node::canSleepFromIdling() { return true; }
    bool C1_Node::canSleepFromTransmitting() { return true; }
    bool C1_Node::canSleepFromListening() { return true; }
    bool C1_Node::canSleepFromSleeping() { return true; }


