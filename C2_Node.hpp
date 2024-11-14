#pragma once
#include "Node.hpp"


class C2_Node : public Node {

public :
    C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex) {


    };

    int getClassId() const  {
        return 2;
    }
    
    std::string initMessage() const override;
    void run() override;
    protected:

    
};
