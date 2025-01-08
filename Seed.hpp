#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Node.hpp"
#include "Logger.hpp"
#include <utility> // For std::pair
#include "C3/C3_Node.hpp"
#include "C2/C2_Node.hpp"
#include "C1/C1_Node.hpp"



struct C2_Node_Mesh_Parameter{
    std::pair<int,int> coordinates;
    unsigned int hopCount;
    uint16_t nextNodeIdInPath;
};

class Seed {

public:
    Seed(std::string communicationWindow, std::string topology,Logger& logger,std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,int64_t  baseTime) : 
    communicationWindow(communicationWindow), logger(logger), 
    topology(topology), dispatchCv(dispatchCv), 
    dispatchCvMutex(dispatchCvMutex), baseTime(baseTime) {

        initializeNodes();
        
    }

    std::vector<std::shared_ptr<Node>> transferOwnership();
  

private:
    Logger& logger;
    std::string communicationWindow;
    std::string topology;
    std::vector<std::shared_ptr<Node>> listNode;
    std::condition_variable& dispatchCv;
    std::mutex& dispatchCvMutex;
    int64_t  baseTime;

    void initializeNodes();

        //TODO: there is bug: the first node must have an ID=0, (we should be able to attribute any ID to the nodes)
    #if COMMUNICATION_PERIOD == RRC_DOWNLINK || COMMUNICATION_PERIOD == RRC_BEACON

    void initialize_RRC_Downlink_Line();
    void initialize_RRC_Beacon_Line();
    void initialize_RRC_Beacon_Mesh();
    void initialize_RRC_Beacon_Mesh_Self_Healing();
    #elif COMMUNICATION_PERIOD == RRC_UPLINK

    void initialize_RRC_Uplink_Mesh();
    void initialize_RRC_Uplink_Line();
    #endif
};
