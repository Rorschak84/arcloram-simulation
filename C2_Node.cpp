#include "C2_Node.hpp"

std::string C2_Node::initMessage() const{

    std::string msg=Node::initMessage();
   std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}

    void C2_Node::run()
{


    // Generate a random duration
    int milliseconds = 1000;
//do we need this while? We have an event driven mechanism 

    while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); // Simulate work
           
        }
}
