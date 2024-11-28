#include "Node.hpp"
#include <optional>

// if this becomes too messy, think about creating an object to populate the node
Node::Node(int id, Logger& logger, std::pair<int,int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex)
    : nodeId(id), running(true),logger(logger), coordinates(coordinates), dispatchCv(dispatchCv), dispatchCvMutex(dispatchCvMutex) {
    // Constructor implementation
    timeOnAirEnd={std::chrono::steady_clock::now()};//for interference, this is the new reference point
   
   
   // endReceivingTimePoint={std::chrono::system_clock::now()};

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
    // state transition name convention: Proposed state -> Current state -> Condition check function


        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Listening}] = [this]() { return canTransmitFromListening(); };
        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Transmitting}] = [this]() { return canTransmitFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Sleeping}] = [this]() { return canTransmitFromSleeping(); };
        stateTransitions[{WindowNodeState::CanTransmit, NodeState::Communicating}] = [this]() { return canTransmitFromCommunicating(); };

        stateTransitions[{WindowNodeState::CanListen, NodeState::Transmitting}] = [this]() { return canListenFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanListen, NodeState::Listening}] = [this]() { return canListenFromListening(); };
        stateTransitions[{WindowNodeState::CanListen, NodeState::Sleeping}] = [this]() { return canListenFromSleeping(); };
        stateTransitions[{WindowNodeState::CanListen, NodeState::Communicating}] = [this]() { return canListenFromCommunicating(); };

        stateTransitions[{WindowNodeState::CanSleep, NodeState::Transmitting}] = [this]() { return canSleepFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanSleep, NodeState::Listening}] = [this]() { return canSleepFromListening(); };
        stateTransitions[{WindowNodeState::CanSleep, NodeState::Sleeping}] = [this]() { return canSleepFromSleeping(); };
        stateTransitions[{WindowNodeState::CanSleep, NodeState::Communicating}] = [this]() { return canSleepFromCommunicating(); };

        stateTransitions[{WindowNodeState::CanCommunicate, NodeState::Transmitting}] = [this]() { return canCommunicateFromTransmitting(); };
        stateTransitions[{WindowNodeState::CanCommunicate, NodeState::Listening}] = [this]() { return canCommunicateFromListening(); };
        stateTransitions[{WindowNodeState::CanCommunicate, NodeState::Sleeping}] = [this]() { return canCommunicateFromSleeping(); };
        stateTransitions[{WindowNodeState::CanCommunicate, NodeState::Communicating}] = [this]() { return canCommunicateFromCommunicating(); };
}


NodeState Node::convertWindowNodeStateToNodeState(WindowNodeState state) {
        switch (state) {
            case WindowNodeState::CanCommunicate:
                return NodeState::Communicating;
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
                    //we cannot do this, we need to change the currentstate before performing the operation related to the proposed state that is valid
                    //currentState = convertWindowNodeStateToNodeState(proposedState);

                    //Log Msg is in the callback function bc we might provide additional info.

                }catch(const std::invalid_argument& e){
                    Log invalidArg("Node "+std::to_string(nodeId)+"cannot convert proposed window state to Node State: current Node Sate."+e.what(), true);
                    logger.logMessage(invalidArg);
                    }
                //we put this in the stae transition function to make the logs chronological
                //we let the others logs in this function to not overload the state transition function with same logs
                //  Log transitionLog("Node "+std::to_string(nodeId)+" transitioned to "+stateToString(currentState), true);
                //     logger.logMessage(transitionLog);   
             } else {

                Log failedTransitionLog("Node "+std::to_string(nodeId)+" transition from "+stateToString(currentState)+" to "+stateToString(proposedState)+ " failed", true);
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
            case NodeState::Communicating: return " Communicating";
            case NodeState::Transmitting: return " Transmiting";
            case NodeState::Listening: return " Listening";
            case NodeState::Sleeping: return " Sleeping";
            default: return "Unknown";
        }
    }

std::string Node::stateToString(WindowNodeState state) {
    switch (state) {
        case WindowNodeState::CanTransmit: return "Transmit";
        case WindowNodeState::CanListen: return " Listen";
        case WindowNodeState::CanCommunicate: return " Communicate";
        case WindowNodeState::CanSleep: return " Sleep";
        default: return "Unknown";
    }
}




//simulate the reception of a message, including potential interferences
void Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir) {

        if(isReceiving){
            //two simultaneous messages are received -> interference
            hadInterference=true;
            Log abortLog("Node "+std::to_string(nodeId)+"aborts Msg reception: "+packet_to_binary(message)+" due to interference", true);
            logger.logMessage(abortLog);
            endReceivingTimePoint = std::max(endReceivingTimePoint.load(), std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) +timeOnAir);
            interferenceCv.notify_one();//notify the thread that simulate the reception of the first message to change the time until it should wait
            return;
        }
        else{ //no interference
            isReceiving=true;
            endReceivingTimePoint= std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            endReceivingTimePoint=endReceivingTimePoint.load()+timeOnAir;

            //launch a thread in detach mode that simulates the reception of the message. Any additional message received during a period of TimeOnAir ms will result in droping of the msg
            std::thread([this, message]() {
                std::unique_lock<std::mutex> lock(interferenceMutex);
                while( std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())< endReceivingTimePoint.load()){
                    interferenceCv.wait_until(lock, endReceivingTimePoint.load());
                }
                if(hadInterference){
                    Log abortLog("Node "+std::to_string(nodeId)+"aborts initial Msg reception: "+packet_to_binary(message)+" due to interference", true);
                    logger.logMessage(abortLog);
                    hadInterference=false;//we drop the initial message
                    isReceiving=false;
                    return;
                }
                else{
                    std::lock_guard<std::mutex> lock(receiveMutex);
                    receiveBuffer.push(message); 
                    Log receivedLog("Node "+std::to_string(nodeId)+" received "+packet_to_binary(message), true);
                    logger.logMessage(receivedLog); 
                    isReceiving=false;
                }

            }).detach();
        }







        // auto now = std::chrono::steady_clock::now();
        // auto newEndTime = now + timeOnAir;
        // // Check for interference
        // if (now < timeOnAirEnd.load()) {


           

        //     // Signal the worker thread to stop
        //     stopReceiving = true;

        //     // Extend the Time On Air window to the furthest point in time (the sender keeps sending the message regardless of the interference detected at the receiver side !)
        //     timeOnAirEnd.store(std::max(timeOnAirEnd.load(), newEndTime));
        //     return;
        // }
        // // No interference: Update the Time On Air end time
        // timeOnAirEnd.store(newEndTime);
        // stopReceiving = false; // Reset stop signal for the new reception

        // // Start a worker thread for Time On Air simulation in detached mode
        // std::thread([this, message, timeOnAir]() {
        //     auto start = std::chrono::steady_clock::now();
        //     auto end = start + timeOnAir;

        //     // Simulate Time On Air with a loop
        //     while (std::chrono::steady_clock::now() < end) {
        //         if (stopReceiving) {
        //             // Interruption detected: Stop processing this message
        //              Log abortLog("Node "+std::to_string(nodeId)+"aborts initial Msg reception: "+packet_to_binary(message)+" due to interference", true);
        //             logger.logMessage(abortLog);
        //             stopReceiving = false; // Reset the stop signal
        //             return;
        //         }
        //         //TODO: declare a global variable that will manage this ticks intervals!
        //         std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid busy-waiting
        //     }

        //     //sometimes the thread scheduler makes the worker thread "misses" the condition on the time on ai
        //     //so we need to check if the stop signal is set
        //     if (stopReceiving) {
        //             // Interruption detected: Stop processing this message
        //              Log abortLog("Node "+std::to_string(nodeId)+"aborts Msg reception: "+packet_to_binary(message)+" due to interference", true);
        //             logger.logMessage(abortLog);
        //             stopReceiving = false; // Reset the stop signal
        //             return;
        //     }


        //     // If no interference occurred, the message is successfully received
        //     {
        //         std::lock_guard<std::mutex> lock(receiveMutex);
        //         //should be replaced by stack.
        //         receiveBuffer.push(message); 
        //     }

        //     //this log will be printed in the receive func of the child class, containing additionnal behaviour    
        //     //  Log receivedLog("Node "+std::to_string(nodeId)+" received "+packet_to_binary(message), true);
        //     //  logger.logMessage(receivedLog); 
        // }).detach(); // Detach the thread so it runs independently
  
}

std::optional<std::vector<uint8_t>> Node::getNextReceivedMessage() { //optionnal is a way to return a value or nothing
    std::lock_guard<std::mutex> lock(receiveMutex);
    if (receiveBuffer.empty()) {
        return std::nullopt; //return nothing
    }
    std::vector<uint8_t> message = receiveBuffer.front();
    
    //change in order to implement node behavior
    receiveBuffer.pop();
    return message;
}

void Node::addMessageToTransmit(const std::vector<uint8_t> message,std::chrono::milliseconds timeOnAir) {
    {       //we add the msg to the buffer, but we need to lock the buffer before
        std::lock_guard<std::mutex> lock(transmitMutex);
        transmitBuffer.push(std::make_pair(message, timeOnAir));
    }

    {
  Log queuedLog("Node "+std::to_string(nodeId)+" queued "+packet_to_binary( message)+" TOA:"+std::to_string(timeOnAir.count()), true);
  logger.logMessage(queuedLog);
   //std::lock_guard<std::mutex> lockNode(dispatchCvMutex); // Lock the shared mutex for condition variable signaling
    // Notify that a new message is ready
    dispatchCv.notify_one();//notify the simulation manager that a message is ready        
    }
}

std::optional<std::pair<std::vector<uint8_t>, std::chrono::milliseconds>>  Node::getNextTransmittingMessage() {//this is called by the transmission loop
    std::lock_guard<std::mutex> lock(transmitMutex);
    if (transmitBuffer.empty()) {
        return std::nullopt;
    }
    std::pair<std::vector<uint8_t>,std::chrono::milliseconds> message_TOA = transmitBuffer.front();
   
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

