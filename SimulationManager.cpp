#include "SimulationManager.hpp"
#include <iostream>

SimulationManager::SimulationManager(int nbNodes)
    : nbNodes(nbNodes) {

}

void SimulationManager::startSimulation() {
   
    for(std::shared_ptr<Node> node : nodes){
        std::thread t(&Node::run, node);
        t.detach();
    }

}

void SimulationManager::stopSimulation() {
    for(std::shared_ptr<Node> node : nodes){
        node->stop();
    }
}

void SimulationManager::registerNode(std::shared_ptr<Node> node) {
    if (node == nullptr) {
    throw std::invalid_argument("Cannot register a nullptr node");
    }
    std::lock_guard<std::mutex> lock(terminalMutex);//his line locks the managerMutex for the duration of the scope, ensuring that no other thread can modify nodes simultaneously
    nodes.push_back(node);

    
    
}


void SimulationManager::printMessage(const std::string& message) {
    // Lock the mutex before writing to the terminal
    //std::lock_guard<std::mutex> lock(terminalMutex);
    std::cout << message << std::endl;
}
