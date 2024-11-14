#include "Node.hpp"
#include <optional>

// if this becomes too messy, think about creating an object to populate the node
Node::Node(int id, Logger& logger, std::pair<int,int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : nodeId(id), running(true),logger(logger), coordinates(coordinates), dispatchCv(dispatchCv), dispatchCvMutex(dispatchCvMutex) {
    // Constructor implementation
}




std::string Node::initMessage() const{
    //retrieve the thread id
    std::thread::id this_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << this_id;
    std::string threadIdStr = ss.str();

    return "Node "+std::to_string(nodeId)+" located at ("+std::to_string(coordinates.first) +","+std::to_string(coordinates.second)+ ")";
}



void Node::stop() {

    //check if it works, like making sure threads are stopped etc.. TODO
    running = false;
}


void Node::receiveMessage(const std::string& message) {
    {
    std::lock_guard<std::mutex> lock(receiveMutex);
    receiveBuffer.push(message);
    }
    Log receivedLog("Node "+std::to_string(nodeId)+" received "+message, true);
    logger.logMessage(receivedLog);
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
    {       //we add the msg to the buffer, but we need to lock the buffer before
        std::lock_guard<std::mutex> lock(transmitMutex);
        transmitBuffer.push(message);
    }

    {
  Log queuedLog("Node "+std::to_string(nodeId)+" queued "+message, true);
  logger.logMessage(queuedLog);
   //std::lock_guard<std::mutex> lockNode(dispatchCvMutex); // Lock the shared mutex for condition variable signaling
    // Notify that a new message is ready
    dispatchCv.notify_one();//notify the simulation manager that a message is ready        
    }
}

std::optional<std::string> Node::getNextTransmittingMessage() {//this is called by the transmission loop
    std::lock_guard<std::mutex> lock(transmitMutex);
    if (transmitBuffer.empty()) {
        return std::nullopt;
    }
    std::string message = transmitBuffer.front();
    transmitBuffer.pop();
    Log queuedLog("Node "+std::to_string(nodeId)+" poped "+message, true);
    logger.logMessage(queuedLog);
    return message;
}

bool Node::hasNextTransmittingMessage() {//this is called by the transmission loop
    std::lock_guard<std::mutex> lock(transmitMutex);
    if (transmitBuffer.empty()) {
        return false;
    }
    return true;
}