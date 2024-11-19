#pragma once
#include "Node.hpp"


class C3_Node : public Node {

public :
    C3_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex) {

            initializeTransitionMap();                 
            setInitialState(NodeState::Sleeping);
    };

    int getClassId() const  {
        return 3;
    }

    std::string initMessage() const override;

protected:
    bool canIdleFromTransmitting();
    bool canIdleFromListening();
    bool canIdleFromSleeping();
    bool canIdleFromIdling();

    bool canTransmitFromIdling();
    bool canTransmitFromListening();
    bool canTransmitFromSleeping();
    bool canTransmitFromTransmitting();

    bool canListenFromIdling();
    bool canListenFromTransmitting();
    bool canListenFromSleeping();
    bool canListenFromListening();

    bool canSleepFromIdling();
    bool canSleepFromTransmitting();
    bool canSleepFromListening();
    bool canSleepFromSleeping();
};
