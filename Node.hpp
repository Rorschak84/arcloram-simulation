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
#include <cmath> // For std::sqrt
#include <random>
#include <sstream>
#include "Log.cpp"



class Node {
public:

    Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex);
    virtual ~Node() = default;
    
    virtual void run()=0; //we need to implement behaviour in child classes
    
    
    void stop();
     
    virtual std::string initMessage() const;

    //used by simulation manager
    void receiveMessage(const std::string& message);//add a message to the receiving buffer

   //for debugging
    std::optional<std::string> getNextTransmittingMessage(); // Method to retrieve a message from the transmitting buffer
    std::optional<std::string> getNextReceivedMessage();// .... from the receiving buffer

    bool hasNextTransmittingMessage() ;//this is called by the transmission loop

    //getters
    int getId() const { return nodeId; }
    int getXCoordinate() const {
        return coordinates.first;
    }

    int getYCoordinate() const {
        return coordinates.second;
    }

protected:

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

    // Condition variable to notify the simulation manager of new messages
    std::condition_variable& dispatchCv;
    std::mutex& dispatchCvMutex;
   
//methods
    //the node adds a message to the transmitting buffer and notifies the simulation manager
    void addMessageToTransmit(const std::string& message);//add a message to the transmitting buffer, this method is private because it is only called by the node itself
};

