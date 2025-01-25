#include "SimulationManager.hpp"
#include <iostream>

SimulationManager::SimulationManager( double distanceThreshold,Logger& logger) 
    : logger(logger),distanceThreshold(distanceThreshold) {

}

void SimulationManager::startSimulation() {


    //launch the thread that will handle the transmission
    startTransmissionLoop();

    // Calculate reachable nodes for each node (BASIC PHY LAYER)
   reachableNodesPerNode = getReachableNodesForAllNodes();

    for(std::shared_ptr<Node> node : nodes){
        
        /*We shifted to an event driven model, nodes are no longer threads running independently
          They wait for an event ( scheduler proposes a state transition) 
          and eventually send a message on their transmission buffer that will be collected by the transmission loop

          std::thread t(&Node::run, node);
           t.detach();
        */ 
        Log initialNodeLog(node->initMessage(), true);
        logger.logMessage(initialNodeLog);
    }



}

void SimulationManager::stopSimulation() {

    for(std::shared_ptr<Node> node : nodes){
        node->stop();
    }
}

void SimulationManager::takeOwnership(std::vector<std::shared_ptr<Node>> nodes)
{
    for (auto& node : nodes) {
        registerNode(node);
    }
}

void SimulationManager::registerNode(std::shared_ptr<Node> node) {
    if (node == nullptr) {
    throw std::invalid_argument("Cannot register a nullptr node");
    }
    nodes.push_back(std::move(node));
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
    logger.logMessage(Log("All reachable nodes calculated:", true));
    for (size_t i = 0; i < allReachableNodes.size(); i++) {
        std::string msg = "Node " + std::to_string(i) + " can reach: ";
        for (const auto& reachableNode : allReachableNodes[i]) {
            msg += std::to_string(reachableNode->getId()) + " ";
        }
        logger.logMessage(Log(msg, true));
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
    //
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

         return (checkForMessages()|| !dispatchRunning); });

        // Log notified("After Wait, condition met", true);
        // logger.logMessage(notified); 
        ProcessMessages();
    }
}

bool SimulationManager::    checkForMessages(){

// Check if there is at least one node with a message to transmit
            for (const auto& node : nodes) {
                if (node->hasNextTransmittingMessage()) {
                    return true;//there is at least one, we exit the loop and process the messages
                }
            }
            return false;//no message to transmit
}

void SimulationManager::ProcessMessages(){
        for (auto& node : nodes) {
            std::optional<std::pair<std::vector<uint8_t>, std::chrono::milliseconds>> message = node->getNextTransmittingMessage();
            if (message.has_value()) {
                // Log hasValue("Node "+std::to_string(node->getId())+" msg is processed", true);
                //  logger.logMessage(hasValue);
                for ( auto& reachableNode : reachableNodesPerNode[node->getId()]) {

                    // Launch a thread in detached mode
                    std::thread([reachableNode,message]() {
                        reachableNode->receiveMessage(message->first,message->second);
                    }).detach();


                }
            }
        }
}

