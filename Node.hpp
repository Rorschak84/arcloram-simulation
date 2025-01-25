#pragma once



#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Logger.hpp"
#include <utility> // For std::pair
#include <functional> // For std::function
#include <optional>
#include <cmath> // For std::sqrt
#include <random>
#include <sstream>
#include <map>
#include <atomic>
#include "PacketTool.hpp"



// Enum representing possible states for nodes
enum class NodeState {
    Transmitting,
    Listening,
    Sleeping,
    Communicating
};

//Enum representing the scheduler proposed states for the nodes
enum class WindowNodeState{
    
    CanTransmit,
    CanListen,
    CanSleep,
    CanCommunicate //mixed state of CanTransmit and CanListen
};

class Node {
public:

    Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,double batteryLevel=2.0);
    virtual ~Node() {
        stopReceiving = true; // Ensure any active threads are signaled to stop
    }

    //virtual void run()=0; //we shifted to an event driven model
    
    void stop();
     

    virtual std::string initMessage() const;//default message to be logged when the node starts

    //used by simulation manager

    //add a message to the receiving buffer if no interference is detected, additionnal behaviour can be added in child classes
    virtual bool receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir);
    std::optional<std::pair<std::vector<uint8_t>,std::chrono::milliseconds>> getNextTransmittingMessage(); // Method to retrieve a message from the transmitting buffer
    // std::optional<std::vector<uint8_t>> getNextReceivedMessage();// .... from the receiving buffer
    bool hasNextTransmittingMessage() ;//this is called by the transmission loop

    //getters
    int getId() const { return nodeId; }
    int getXCoordinate() const {
        return coordinates.first;
    }
    int getYCoordinate() const {
        return coordinates.second;
    }

     //add TDMA
     void addActivation( int64_t activationTime, WindowNodeState activationState);
             
    const std::vector<std::pair<int64_t, WindowNodeState>>& getActivationSchedule() const {
        return activationSchedule;
    }

    void onTimeChange(WindowNodeState proposedState);
     virtual  int getClassId() const =0;

protected:


    double batteryLevel=3.0;

    std::pair<int, int> coordinates ={0,0};//in meters (x,y)
    int nodeId;
    bool running;
    Logger& logger;

    // Buffers transmitting messages
    std::queue<std::pair<std::vector<uint8_t>,std::chrono::milliseconds >> transmitBuffer;//MSG + Time On Air (TOA)
 
    //to simulate interferences:
    //old 
    std::atomic<std::chrono::steady_clock::time_point> timeOnAirEnd; // End of current Time On Air
    std::atomic<bool> stopReceiving{false};           // Signals the active thread to stop
    //new
    std::atomic<bool> isReceiving=false;
    std::atomic<bool>  hadInterference=false;
     // Define a time_point with millisecond precision
    std::atomic<std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>>endReceivingTimePoint;
    std::mutex interferenceMutex;
    std::condition_variable interferenceCv;


    // Mutexes to protect the buffers
    std::mutex receiveMutex;
    std::mutex transmitMutex;

    // Condition variable to notify the simulation manager of new messages
    std::condition_variable& dispatchCv;
    std::mutex& dispatchCvMutex;

    //methods
    //the node adds a message to the transmitting buffer and notifies the simulation manager
    void addMessageToTransmit(const std::vector<uint8_t> message,const std::chrono::milliseconds timeOnAir);//add a message to the transmitting buffer, this method is private because it is only called by the node itself


    //---------------------------------------TDMA-------------------------------------
    std::vector<std::pair<int64_t, WindowNodeState>> activationSchedule; // the list of proposed node state (window State) by the scheduler at a given time
    NodeState currentState; // the actual state of the node
    // Transition rules using functions for complex conditions: link a proposed state/current State with a callback that will check conditions and eventually change current state and perform actions
    std::map<std::pair<WindowNodeState, NodeState>,  std::function<bool()>> stateTransitions;
      
    void setInitialState(NodeState initialState) {
        currentState = initialState;
    }
    NodeState convertWindowNodeStateToNodeState(WindowNodeState state);

   
    static std::string stateToString(NodeState state);
    static std::string stateToString(WindowNodeState state);


    //we need to define in child classes the state machine
    //convention for the name of the methods:
    //proposedWindowStateFromCurrentState: IdleFromTransmit

    virtual bool canTransmitFromListening()=0;
    virtual bool canTransmitFromSleeping()=0;
    virtual bool canTransmitFromTransmitting()=0;
    virtual bool canTransmitFromCommunicating()=0;

    virtual bool canListenFromTransmitting()=0;
    virtual bool canListenFromSleeping()=0;
    virtual bool canListenFromListening()=0;
    virtual bool canListenFromCommunicating()=0;

    virtual bool canSleepFromTransmitting()=0;
    virtual bool canSleepFromListening()=0;
    virtual bool canSleepFromSleeping()=0;
    virtual bool canSleepFromCommunicating()=0;

    virtual bool canCommunicateFromTransmitting()=0;
    virtual bool canCommunicateFromListening()=0;
    virtual bool canCommunicateFromSleeping()=0;
    virtual bool canCommunicateFromCommunicating()=0;
   
    void initializeTransitionMap();

};
