#include "Node.hpp"
#include "SimulationManager.hpp"
#include <sstream>

Node::Node(int id)
    : nodeId(id), running(true) {
    // Constructor implementation
}

void Node::run() {
    std::thread::id this_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << this_id;
    std::string threadIdStr = ss.str();

    manager->printMessage("Thread: "+threadIdStr+ " for Node "+std::to_string(nodeId)+" is running.");
  
    while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Simulate work
            manager->printMessage("Node "+std::to_string(nodeId)+" wakes up.");
        
        }
}

void Node::stop() {
    running = false;
}
