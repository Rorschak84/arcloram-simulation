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
#include "Log.cpp"
#include <map>


// Enum representing possible states for nodes
enum class NodeState {
    Idling,
    Transmitting,
    Listening,
    Sleeping
};

enum class WindowNodeState{
    CanIdle,
    CanTransmit,
    CanListen,
    CanSleep
};

class Node {
public:

    Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex);
    virtual ~Node() = default;
    
    //virtual void run()=0; //we shifted to an event driven model
    
    
    void stop();
     
    virtual std::string initMessage() const;//default message to be logged when the node starts

    //used by simulation manager
    void receiveMessage(const std::string& message);//add a message to the receiving buffer
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

     //add TDMA
     void addActivation(int64_t activationTime, WindowNodeState activationState);
             

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


    //---------------------------------------TDMA-------------------------------------
      std::vector<std::pair<int64_t, WindowNodeState>> activationSchedule; // the proposed node state by the scheduler at a given time
    NodeState currentState; // the actual state of the node
    // Transition rules using functions for complex conditions: link a proposed state/current State with a callback that will check conditions and eventually change current state and perform actions
    std::map<std::pair<WindowNodeState, NodeState>,  std::function<bool()>> stateTransitions;
      
    void setInitialState(NodeState initialState) {
        currentState = initialState;
    }
    NodeState convertWindowNodeStateToNodeState(WindowNodeState state);

    void onTimeChange(WindowNodeState proposedState);
    
    const std::vector<std::pair<int64_t, WindowNodeState>>& getActivationSchedule() const {
        return activationSchedule;
    }
    static std::string stateToString(NodeState state);


    //we need to define in child classes the state machine
    //convention for the name of the methods:
    //proposedWindowStateFromCurrentState: IdleFromTransmit
    
    virtual bool canIdleFromTransmitting()=0;
    virtual bool canIdleFromListening()=0;
    virtual bool canIdleFromSleeping()=0;
    virtual bool canIdleFromIdling()=0;

    virtual bool canTransmitFromIdling()=0;
    virtual bool canTransmitFromListening()=0;
    virtual bool canTransmitFromSleeping()=0;
    virtual bool canTransmitFromTransmitting()=0;

    virtual bool canListenFromIdling()=0;
    virtual bool canListenFromTransmitting()=0;
    virtual bool canListenFromSleeping()=0;
    virtual bool canListenFromListening()=0;

    virtual bool canSleepFromIdling()=0;
    virtual bool canSleepFromTransmitting()=0;
    virtual bool canSleepFromListening()=0;
    virtual bool canSleepFromSleeping()=0;

    void initializeTransitionMap();

};
