#pragma once
#include "Node.hpp"


class C1_Node : public Node {

public :
    C1_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex) {

            initializeTransitionMap();
            setInitialState(NodeState::Sleeping);
    };

    int getClassId() const  {
        return 1;
    }
    
    std::string initMessage() const override;

  
    protected:

   

};
