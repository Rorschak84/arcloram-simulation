#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}

    bool C3_Node::canIdleFromTransmitting() { return true; }
    bool C3_Node::canIdleFromListening() { return true; }
    bool C3_Node::canIdleFromSleeping() { return true; }
    bool C3_Node::canIdleFromIdling() { return true; }

    bool C3_Node::canTransmitFromIdling() { return true; }
    bool C3_Node::canTransmitFromListening() { return true; }
    bool C3_Node::canTransmitFromSleeping() { return true; }
    bool C3_Node::canTransmitFromTransmitting() { return true; }

    bool C3_Node::canListenFromIdling() { return true; }
    bool C3_Node::canListenFromTransmitting() { return true; }
    bool C3_Node::canListenFromSleeping() { return true; }
    bool C3_Node::canListenFromListening() { return true; }

    bool C3_Node::canSleepFromIdling() { return true; }
    bool C3_Node::canSleepFromTransmitting() { return true; }
    bool C3_Node::canSleepFromListening() { return true; }
    bool C3_Node::canSleepFromSleeping() { return true; }
