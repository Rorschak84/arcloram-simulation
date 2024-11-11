#include "Node.hpp"
#include <optional>

// if this becomes too messy, think about creating an object to populate the node
Node::Node(int id, Logger& logger, std::pair<int,int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : nodeId(id), running(true),logger(logger), coordinates(coordinates), dispatchCv(dispatchCv), dispatchCvMutex(dispatchCvMutex) {
    // Constructor implementation
}

void Node::run() {

    //retrieve the thread id
    std::thread::id this_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << this_id;
    std::string threadIdStr = ss.str();

    //initial message
    Log initialNodeLog("Node "+std::to_string(nodeId)+" located at ("+std::to_string(coordinates.first) +","+std::to_string(coordinates.second)+ ") is running.", false);
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

void Node::stop() {

    //check if it works
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