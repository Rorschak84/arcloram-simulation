#include "SimulationManager.hpp"
#include <iostream>

SimulationManager::SimulationManager(int nbNodes, double distanceThreshold) 
    : nbNodes(nbNodes),distanceThreshold(distanceThreshold) {

}

void SimulationManager::startSimulation() {
   
   //launch the threads corresponding to the nodes in detached mode
    for(std::shared_ptr<Node> node : nodes){
        std::thread t(&Node::run, node);
        t.detach();
    }

    //launch the thread that

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


// Calculate Euclidean distance between two nodes
double SimulationManager::calculateDistance(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
    double dx = a->getXCoordinate() - b->getXCoordinate();
    double dy = a->getYCoordinate() - b->getYCoordinate();
    return std::sqrt(dx * dx + dy * dy);
}


// Function to get pointers to nodes reachable from a given node
std::vector<std::shared_ptr<Node>> SimulationManager::getReachableNodesForNode(const std::shared_ptr<Node>& node) {
    std::vector<std::shared_ptr<Node>> reachableNodes;
    for (const auto& otherNode : nodes) {
        if (node != otherNode) { // Exclude itself
            double distance = calculateDistance(node, otherNode);
            if (distance <= distanceThreshold) {
                reachableNodes.push_back(otherNode);
            }
        }
    }
    return reachableNodes;
}

// Function to get reachable nodes for all nodes
std::vector<std::vector<std::shared_ptr<Node>>> SimulationManager::getReachableNodesForAllNodes() {
    std::vector<std::vector<std::shared_ptr<Node>>> allReachableNodes;
    for (const auto& node : nodes) {
        allReachableNodes.push_back(getReachableNodesForNode(node));
    }
    return allReachableNodes;
}