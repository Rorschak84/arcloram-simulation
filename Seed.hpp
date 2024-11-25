#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Node.hpp"
#include "Logger.hpp"
#include <utility> // For std::pair
#include "C3_Node.hpp"
#include "C2_Node.hpp"
#include "C1_Node.hpp"

/*
                USE CASES:
There is two major components:
-the topology (position of nodes)
-the Communication Window (Ex: RRC_Beacon, RRC_Downlink, ENC_Beacon), that will dictate the classes of nodes



The seed class will be used to define the topology and the communication window, automatically provisioning the required elements to show the functionnalities of the protocol


Use cases Description:
    For RRC:
            -Topologies:
                -Line
                -Star
                -Mesh

            -Communication Window:
                -RRC_Beacon
                -RRC_Downlink (Or RRC_Flood, neet to set a name for good)
                -RRC_Uplink  (same)   
    For ENC:
            -Topologies:
                -Star 
            
            -Communication Window:
                -ENC_Beacon
                -ENC_Downlink
                -ENC_Uplink

If time allows, we will consider an hybrid use case that will combine the two protocols



Trick: //TODO: use the math online tool that was free when in college to generate the coordinates of the nodes (they have to be in range)

*/  





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
    std::vector<std::pair<int, int>> nodeCoordinates;
    std::vector<std::shared_ptr<Node>> listNode;
    std::condition_variable& dispatchCv;
    std::mutex& dispatchCvMutex;
    int64_t  baseTime;

    void initializeNodes();
    void initialize_RRC_Beacon_Line();
};
