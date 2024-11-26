#include "C1_Node.hpp"


std::string C1_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}



    bool C1_Node::canTransmitFromListening() { return true; }
    bool C1_Node::canTransmitFromSleeping() { return true; }
    bool C1_Node::canTransmitFromTransmitting() { return true; }

    bool C1_Node::canTransmitFromCommunicating()
    {
        return true;
    }

    bool C1_Node::canListenFromTransmitting() { return true; }
    bool C1_Node::canListenFromSleeping() { return true; }
    bool C1_Node::canListenFromListening() { return true; }

    bool C1_Node::canListenFromCommunicating()
    {
        return true;
    }

    bool C1_Node::canSleepFromTransmitting() { return true; }
    bool C1_Node::canSleepFromListening() { return true; }
    bool C1_Node::canSleepFromSleeping() { return true; }

    bool C1_Node::canSleepFromCommunicating()
    {
        return true;
    }

    bool C1_Node::canCommunicateFromTransmitting()
    {
        return true;
    }

    bool C1_Node::canCommunicateFromListening()
    {
        return true;
    }

    bool C1_Node::canCommunicateFromSleeping()
    {
        return true;
    }

    bool C1_Node::canCommunicateFromCommunicating()
    {
        return true;
    }
