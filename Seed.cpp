#include "Seed.hpp"




    std::vector<std::shared_ptr<Node>> Seed::transferOwnership() {
        std::vector<std::shared_ptr<Node>> allNodes;

        for (auto& node : listNode) {
            allNodes.push_back(std::move(node)); //std::move transfers ownership of the shared_ptr (from listNode to allNodes) without copying.
        }
        listNode.clear(); // Clear the seed's copy of the nodes
        return allNodes;
    }

    void Seed::initializeNodes()
    {
        std::string use_case= communicationWindow +"_"+ topology;

        if(use_case=="RRC_Beacon_Line"){
            initialize_RRC_Beacon_Line();
        }
    }

    void Seed::initialize_RRC_Beacon_Line(){
        //create a C3 node
        std::pair<int, int> coordinates = std::make_pair(0, 0);
        auto firstNode = std::make_shared<C3_Node>(0, logger, nodeCoordinates[0],dispatchCv,dispatchCvMutex);
        firstNode->addActivation(baseTime+2000, WindowNodeState::CanTransmit); 
        firstNode->addActivation(baseTime+4000, WindowNodeState::CanIdle);
    }
