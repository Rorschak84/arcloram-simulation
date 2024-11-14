#include "C3_Node.hpp"


std::string C3_Node::initMessage() const{

    std::string msg=Node::initMessage();
    std::string finalMsg= msg+ "Class: "+std::to_string(getClassId())+ " started to run";

    return finalMsg;
}

    void C3_Node::run()
{

    //initial message
    Log initialNodeLog(initMessage(), false);
    logger.logMessage(initialNodeLog);
  
    //randomness:

    // Create a random device and a seed for the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    // Define a distribution range for the random time duration (e.g., 100 to 1000 milliseconds)
    std::uniform_int_distribution<> dist(2000, 4000); // Range in milliseconds

    // Generate a random duration
    int randomMilliseconds = dist(gen);

    while (running) {

            std::this_thread::sleep_for(std::chrono::milliseconds(randomMilliseconds)); // Simulate work
            addMessageToTransmit("--MSG from Node "+std::to_string(nodeId)+"--"); // Add a message to the transmit buffer  

        }
}
