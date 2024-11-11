#include "SimulationManager.hpp"
#include <iostream>

SimulationManager::SimulationManager(int nbNodes, double distanceThreshold,Logger& logger) 
    : nbNodes(nbNodes),logger(logger),distanceThreshold(distanceThreshold) {

}

void SimulationManager::startSimulation() {


    //launch the thread that will handle the transmission
    startTransmissionLoop();


   reachableNodesPerNode = getReachableNodesForAllNodes();
   //launch the threads corresponding to the nodes in detached mode
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


void SimulationManager::startTransmissionLoop() {
    dispatchRunning = true;
    dispatchThread = std::thread(&SimulationManager::transmissionLoop, this);
}

void SimulationManager::stopTransmissionLoop() {
    dispatchRunning = false;
    dispatchCv.notify_all(); // Wake up the thread if it's waiting
    if (dispatchThread.joinable()) {
        dispatchThread.join();
    }
}

void SimulationManager::transmissionLoop() {
    while (dispatchRunning) {


        std::unique_lock<std::mutex> lock(dispatchCvMutex);
        // Log waiting("Before Wait", true);
        // logger.logMessage(waiting);

        dispatchCv.wait(lock, 
        [this] {
        // Log notified("Notified, checking condition", true);
        // logger.logMessage(notified);
        //TODO: handle termination of the program

         return (checkForMessages()); });

        // Log notified("After Wait, condition met", true);
        // logger.logMessage(notified); 
        ProcessMessages();
    }
}

bool SimulationManager::checkForMessages(){

// Check if there is at least one node with a message to transmit
            for (const auto& node : nodes) {
                if (node->hasNextTransmittingMessage()) {
                    return true;//there is at least one, we exit the loop and process the messages
                }
            }
            return false;//no message to transmit
}

void SimulationManager::ProcessMessages(){
        for (const auto& node : nodes) {
            std::optional<std::string> message = node->getNextTransmittingMessage();
            if (message.has_value()) {
                Log hasValue("Node "+std::to_string(node->getId())+" msg is processed", true);
                 logger.logMessage(hasValue);
                for (const auto& reachableNode : reachableNodesPerNode[node->getId()]) {
                    reachableNode->receiveMessage(message.value());
                }
            }
        }
}


void SimulationManager::testFunction() {
  
}