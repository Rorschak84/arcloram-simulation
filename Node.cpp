#include "Node.hpp"
#include <sstream>

Node::Node(int id, Logger& logger)
    : nodeId(id), running(true),logger(logger) {
    // Constructor implementation
}

void Node::run() {
    std::thread::id this_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << this_id;
    std::string threadIdStr = ss.str();

    logger.logMessage("Thread: "+threadIdStr+ " for Node "+std::to_string(nodeId)+" is running.");
  
    while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate work
            logger.logMessage("Node "+std::to_string(nodeId)+" wakes up.");   
        }
}

void Node::stop() {
    running = false;
}
