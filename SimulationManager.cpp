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
        // Wait for a notification or stop condition
        dispatchCv.wait(lock, [this] {//wait to be notified by addMessageToTransmit

            // Check if there is at least one node with a message to transmit
            for (const auto& node : nodes) {
                if (node->getNextTransmittingMessage().has_value()) {
                    return true;//there is at least one, we exit the loop and process the messages
                }
            }
            return !dispatchRunning;
        });

        // Process messages if we received a notification
        for (const auto& node : nodes) {
            std::optional<std::string> message = node->getNextTransmittingMessage();
            if (message) {
                // Get reachable nodes for this node
                std::vector<std::shared_ptr<Node>> reachableNodes = getReachableNodesForNode(node);//TODO: use the precalculated values instead

                // Deliver the message to all reachable nodes
                for (const auto& reachableNode : reachableNodes) {
                    reachableNode->receiveMessage(*message);
                    std::cout << "Message transmitted from Node " << node->getId()
                              << " to Node " << reachableNode->getId() << ": " << *message << std::endl;
                }
            }
        }
    }
}