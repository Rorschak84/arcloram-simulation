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


NodeState Node::convertWindowNodeStateToNodeState(WindowNodeState state) {
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
       
       //TODO: should we put mutex to protect shared ressources?
        // Spawn a new thread to handle the state transition asynchronously
         std::thread([this, proposedState]() {    
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
                //we put this in the stae transition function to make the logs chronological
                //we let the others logs in this function to not overload the state transition function with same logs
                //  Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to "+stateToString(currentState), true);
                //     logger.logMessage(transitionLog);   
             } else {

                Log failedTransitionLog("Node "+std::to_string(nodeId)+" transition from "+stateToString(currentState)+" to proposed state failed due to conditions.", true);
               logger.logMessage(failedTransitionLog);
            }
        } else {
            Log noTransitionLog("Node "+std::to_string(nodeId)+" No valid state transition rule found for proposed state "+std::to_string(static_cast<int>(proposedState))+" and current state "+stateToString(currentState), true);
            logger.logMessage(noTransitionLog);

          
        }
         }).detach(); // Detach the thread to let it run independently
}

 std::string Node::stateToString(NodeState state) {
        switch (state) {
            case NodeState::Idling: return "Node Idling";
            case NodeState::Transmitting: return "Node Transmiting";
            case NodeState::Listening: return "Node Listening";
            case NodeState::Sleeping: return "Node Sleeping";
            default: return "Unknown";
        }
    }




//TODO: change everything to chrono + watch the prompt from chatgpt
void Node::receiveMessage(const std::string message, std::chrono::milliseconds timeOnAir) {

        // State Condition: node must be listening to receive a message
        if(currentState!=NodeState::Listening){
            Log interferenceLog("Node "+std::to_string(nodeId)+" is not listening, message "+message+" is lost", true);
            logger.logMessage(interferenceLog);
            return;
        }


        auto now = std::chrono::steady_clock::now();
        auto newEndTime = now + timeOnAir;
        // Check for interference
        if (now < timeOnAirEnd.load()) {
            // Interference detected
            Log interferenceLog("Node: "+std::to_string(nodeId)+ "receives simultaneous Msg! Dropping message: "+message, true);
            logger.logMessage(interferenceLog);
           

            // Signal the worker thread to stop
            stopReceiving = true;

            // Extend the Time On Air window to the furthest point in time (the sender keeps sending the message regardless of the interference detected at the receiver side !)
            timeOnAirEnd.store(std::max(timeOnAirEnd.load(), newEndTime));
            return;
        }
        // No interference: Update the Time On Air end time
        timeOnAirEnd.store(newEndTime);
        stopReceiving = false; // Reset stop signal for the new reception

        // Start a worker thread for Time On Air simulation in detached mode
        std::thread([this, message, timeOnAir]() {
            auto start = std::chrono::steady_clock::now();
            auto end = start + timeOnAir;

            // Simulate Time On Air with a loop
            while (std::chrono::steady_clock::now() < end) {
                if (stopReceiving) {
                    // Interruption detected: Stop processing this message
                     Log abortLog("Node "+std::to_string(nodeId)+"aborts Msg reception: "+message+" due to interference", true);
                    logger.logMessage(abortLog);
                    stopReceiving = false; // Reset the stop signal
                    return;
                }
                //TODO: declare a global variable that will manage this ticks intervals!
                std::this_thread::sleep_for(std::chrono::milliseconds(15)); // Avoid busy-waiting
            }

            //sometimes the thread scheduler makes the worker thread "misses" the condition on the time on ai
            //so we need to check if the stop signal is set
            if (stopReceiving) {
                    // Interruption detected: Stop processing this message
                     Log abortLog("Node "+std::to_string(nodeId)+"aborts Msg reception: "+message+" due to interference", true);
                    logger.logMessage(abortLog);
                    stopReceiving = false; // Reset the stop signal
                    return;
            }


            // If no interference occurred, the message is successfully received
            {
                std::lock_guard<std::mutex> lock(receiveMutex);
                receiveBuffer.push(message); // Add the message to the buffer
            }

            Log receivedLog("Node "+std::to_string(nodeId)+" received "+message, true);
            logger.logMessage(receivedLog); 
        }).detach(); // Detach the thread so it runs independently
  
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

void Node::addMessageToTransmit(const std::string& message,std::chrono::milliseconds timeOnAir) {
    {       //we add the msg to the buffer, but we need to lock the buffer before
        std::lock_guard<std::mutex> lock(transmitMutex);
        transmitBuffer.push(std::make_pair(message, timeOnAir));
    }

    {
  Log queuedLog("Node "+std::to_string(nodeId)+" queued "+message+" TOA:"+std::to_string(timeOnAir.count()), true);
  logger.logMessage(queuedLog);
   //std::lock_guard<std::mutex> lockNode(dispatchCvMutex); // Lock the shared mutex for condition variable signaling
    // Notify that a new message is ready
    dispatchCv.notify_one();//notify the simulation manager that a message is ready        
    }
}

std::optional<std::pair<std::string, std::chrono::milliseconds>>  Node::getNextTransmittingMessage() {//this is called by the transmission loop
    std::lock_guard<std::mutex> lock(transmitMutex);
    if (transmitBuffer.empty()) {
        return std::nullopt;
    }
    std::pair<std::string,std::chrono::milliseconds> message_TOA = transmitBuffer.front();
   
    transmitBuffer.pop();
    return message_TOA;
}

bool Node::hasNextTransmittingMessage() {//this is called by the transmission loop
    std::lock_guard<std::mutex> lock(transmitMutex);
    if (transmitBuffer.empty()) {
        return false;
    }
    return true;
}

