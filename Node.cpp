#include "Node.hpp"
#include <sstream>

Node::Node(int id, Logger& logger, std::pair<int,int> coordinates)
    : nodeId(id), running(true),logger(logger), coordinates(coordinates) {
    // Constructor implementation
}

void Node::run() {
    std::thread::id this_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << this_id;
    std::string threadIdStr = ss.str();

    logger.logMessage("Thread: "+threadIdStr+ " for Node "+std::to_string(nodeId)+" located at ("+std::to_string(coordinates.first) +","+std::to_string(coordinates.second)+ ")is running.");
  
    while (running) {

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Simulate work
            logger.logMessage("Node "+std::to_string(nodeId)+" wakes up.");   


        }
}

void Node::stop() {

    //check if it works
    running = false;
}


void Node::receiveMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(receiveMutex);
    receiveBuffer.push(message);
    logger.logMessage("Node "+std::to_string(nodeId)+" received "+message);
}

std::optional<std::string> Node::getNextReceivedMessage() { //optionnal is a way to return a value or nothing
    std::lock_guard<std::mutex> lock(receiveMutex);
    if (receiveBuffer.empty()) {
        return std::nullopt; //return nothing
    }
    std::string message = receiveBuffer.front();
    
    //change in order to implement node behavior
    receiveBuffer.pop();
    return message;
}

void Node::addMessageToTransmit(const std::string& message) {
    std::lock_guard<std::mutex> lock(transmitMutex);
    transmitBuffer.push(message);
    logger.logMessage("Node "+std::to_string(nodeId)+" queued "+message);
}

std::optional<std::string> Node::getNextTransmittingMessage() {
    std::lock_guard<std::mutex> lock(transmitMutex);
    if (transmitBuffer.empty()) {
        return std::nullopt;
    }
    std::string message = transmitBuffer.front();
    transmitBuffer.pop();
    return message;
}