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
    //running = false; useless since we shifted to event-driven model
    //the event is the global clock
}


void Node::initializeTransitionMap(){
    // Safe to assign the C1/2/3 class's callBack function after construction
    //this function will be called in the constructor of the Child classes ! (C1, C2, C3), otherwise virtual func implementation cannot be solved
    // Define state transition rules: Proposed state -> Current state -> Condition check function
        stateTransitions[{WindowNodeState::CanIdle, NodeState::Transmitting}] = [this]() { return canIdleFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanIdle, NodeState::Listening}] = [this]() { return canIdleFromListening(); };
        stateTransitions[{WindowNodeState::CanIdle, NodeState::Idling}] = [this]() { return canIdleFromIdling(); };
        stateTransitions[{WindowNodeState::CanIdle, NodeState::Sleeping}] = [this]() { return canIdleFromSleeping(); };

        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Idling}] = [this]() { return canTransmitFromIdling(); };
        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Listening}] = [this]() { return canTransmitFromListening(); };
        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Transmitting}] = [this]() { return canTransmitFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Sleeping}] = [this]() { return canTransmitFromSleeping(); };

        stateTransitions[{WindowNodeState::CanListen, NodeState::Idling}] = [this]() { return canListenFromIdling(); };
        stateTransitions[{WindowNodeState::CanListen, NodeState::Transmitting}] = [this]() { return canListenFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanListen, NodeState::Listening}] = [this]() { return canListenFromListening(); };
        stateTransitions[{WindowNodeState::CanListen, NodeState::Sleeping}] = [this]() { return canListenFromSleeping(); };

        stateTransitions[{WindowNodeState::CanSleep, NodeState::Idling}] = [this]() { return canSleepFromIdling(); };
        stateTransitions[{WindowNodeState::CanSleep, NodeState::Transmitting}] = [this]() { return canSleepFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanSleep, NodeState::Listening}] = [this]() { return canSleepFromListening(); };
        stateTransitions[{WindowNodeState::CanSleep, NodeState::Sleeping}] = [this]() { return canSleepFromSleeping(); };
}


NodeState convertWindowNodeStateToNodeState(WindowNodeState state) {
        switch (state) {
            case WindowNodeState::CanIdle:
                return NodeState::Idling;
            case WindowNodeState::CanTransmit:
                return NodeState::Transmitting;
            case WindowNodeState::CanListen:
                return NodeState::Listening;
            case WindowNodeState::CanSleep:
                return NodeState::Sleeping;
            default:
                throw std::invalid_argument("Invalid WindowNodeState for conversion");
        }
    }   

void Node::addActivation(int64_t activationTime, WindowNodeState activationState) {
        //add a new activation time and state to the schedule
        activationSchedule.emplace_back(activationTime, activationState);
    }
             
void Node::onTimeChange(WindowNodeState proposedState) {
       
        auto key = std::make_pair(proposedState, currentState);

        // Check if there is a registered transition function for the proposed and current state
        auto it = stateTransitions.find(key);
        if (it != stateTransitions.end()) {
            // Call the transition function (condition function)
            if (it->second()) {
                // Transition is valid, so update the current state
                try {
                    currentState = convertWindowNodeStateToNodeState(proposedState);
                }catch(const std::invalid_argument& e){
                    Log invalidArg("Node "+std::to_string(nodeId)+"cannot convert proposed window state to Node State: current Node Sate."+e.what(), true);
                    logger.logMessage(invalidArg);
                    }

                 Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to "+stateToString(currentState), true);
                    logger.logMessage(transitionLog);   
             } else {

                Log failedTransitionLog("Node "+std::to_string(nodeId)+" transition from "+stateToString(currentState)+" to proposed state failed due to conditions.", true);
               logger.logMessage(failedTransitionLog);
            }
        } else {
            Log noTransitionLog("Node "+std::to_string(nodeId)+" No valid state transition rule found for proposed state "+std::to_string(static_cast<int>(proposedState))+" and current state "+stateToString(currentState), true);
            logger.logMessage(noTransitionLog);

          
        }
}

static std::string stateToString(NodeState state) {
        switch (state) {
            case NodeState::Idling: return "Node Idling";
            case NodeState::Transmitting: return "Node Transmiting";
            case NodeState::Listening: return "Node Listening";
            case NodeState::Sleeping: return "Node Sleeping";
            default: return "Unknown";
        }
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

