#pragma once



#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Logger.hpp"
#include <utility> // For std::pair
#include <optional>



class Node {
public:

    Node(int id, Logger& logger,std::pair<int, int> coordinates);
    void run();
    void stop();
     

    void receiveMessage(const std::string& message);//add a message to the receiving buffer

   //for debugging
    std::optional<std::string> getNextTransmittingMessage(); // Method to retrieve a message from the transmitting buffer
    std::optional<std::string> getNextReceivedMessage();// .... from the receiving buffer


private:

//variables

    std::pair<int, int> coordinates ={0,0};//in meters (x,y)
    int nodeId;
    bool running;
    Logger& logger;

    // Buffers for receiving and transmitting messages
    std::queue<std::string> receiveBuffer;
    std::queue<std::string> transmitBuffer;
    // Mutexes to protect the buffers
    std::mutex receiveMutex;
    std::mutex transmitMutex;

//methods
    
    void addMessageToTransmit(const std::string& message);//add a message to the transmitting buffer, this method is private because it is only called by the node itself
};

