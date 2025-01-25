#include "Node.hpp"
#include <optional>
#include <future>

// if this becomes too messy, think about creating an object to populate the node
Node::Node(int id, Logger& logger, std::pair<int,int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,double batteryLevel)
    : nodeId(id), running(true),logger(logger), coordinates(coordinates), dispatchCv(dispatchCv), dispatchCvMutex(dispatchCvMutex), batteryLevel(batteryLevel) {
    // Constructor implementation
    timeOnAirEnd={std::chrono::steady_clock::now()};//for interference, this is the new reference point
    
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
bool Node::receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir) {

        if(isReceiving){
            //two simultaneous messages are received -> interference
            hadInterference=true;
            Log abortLog("Node "+std::to_string(nodeId)+"aborts Msg reception: "/*+packet_to_binary(message)*/+" due to interference", true);
            logger.logMessage(abortLog);
            endReceivingTimePoint = std::max(endReceivingTimePoint.load(), std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) +timeOnAir);
            interferenceCv.notify_one();//notify the thread that simulate the reception of the first message to change the time until it should wait
            return false;
        }
        else{ //no interference
            isReceiving=true;
            endReceivingTimePoint= std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            endReceivingTimePoint=endReceivingTimePoint.load()+timeOnAir;

            std::future<bool> resultFuture =std::async(std::launch::async, [this, message](){

 std::unique_lock<std::mutex> lock(interferenceMutex);
                while( std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())< endReceivingTimePoint.load()){
                    interferenceCv.wait_until(lock, endReceivingTimePoint.load());
                }
                if(hadInterference){
                    Log abortLog("Node "+std::to_string(nodeId)+"aborts initial Msg reception: "/*+detailedBytesToString(message)*/+" due to interference", true);
                    logger.logMessage(abortLog);
                    hadInterference=false;//we drop the initial message
                    isReceiving=false;
                    return false;
                }
                else{
                //THE BUFFER IS DEPRECATED, WE DON'T NEED IT ANYMORE TODO
                    std::lock_guard<std::mutex> lock(receiveMutex);
                    // receiveBuffer.push(message); 
                    #if COMMUNICATION_PERIOD == RRC_BEACON || COMMUNICATION_PERIOD == RRC_DOWNLINK
                    Log receivedLog("Node "+std::to_string(nodeId)+" received "+detailedBytesToString(message,common::fieldMap), true);
                    logger.logMessage(receivedLog); 
                    #elif COMMUNICATION_PERIOD == RRC_UPLINK
                    if(message[0]==common::typeData[0]){//data Packet
                    Log receivedLog("Node "+std::to_string(nodeId)+" received "+detailedBytesToString(message,common::dataFieldMap), true);
                    logger.logMessage(receivedLog);

                    } 
                    else if(message[0]==common::typeACK[0]){
                    Log receivedLog("Node "+std::to_string(nodeId)+" received "+detailedBytesToString(message,common::ackFieldMap), true);
                    logger.logMessage(receivedLog);

                    }

                    #endif
                    
                    isReceiving=false;
                    return true;
                }
            });
            return resultFuture.get();
        }
}

void Node::addMessageToTransmit(const std::vector<uint8_t> message,std::chrono::milliseconds timeOnAir) {
    {       //we add the msg to the buffer, but we need to lock the buffer before
        std::lock_guard<std::mutex> lock(transmitMutex);
        transmitBuffer.push(std::make_pair(message, timeOnAir));
    }

    {
    #if COMMUNICATION_PERIOD == RRC_BEACON || COMMUNICATION_PERIOD == RRC_DOWNLINK
    Log queuedLog("Node "+std::to_string(nodeId)+" queued "+detailedBytesToString( message,common::fieldMap), true);
    logger.logMessage(queuedLog);
    #endif
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

