#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}

    void C3_Node::run()
{



    // Generate a random duration
    int randomMilliseconds = 1000;

    while (running) {

            std::this_thread::sleep_for(std::chrono::milliseconds(randomMilliseconds)); // Simulate work
          
        }
}
