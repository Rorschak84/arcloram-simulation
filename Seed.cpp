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

        //for other uses cases, you will probably have to create additional constructors for C3, C2.. to provision the element 

        else{
            throw std::invalid_argument("The use case is not implemented");
        }
    }

    void Seed::initialize_RRC_Beacon_Line(){
       
        //create a C3 node
        std::pair<int, int> coordinates = std::make_pair(0, 0);
        auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates,dispatchCv,dispatchCvMutex);
        firstNode->addActivation(baseTime+2000, WindowNodeState::CanTransmit); 
        firstNode->addActivation(baseTime+4000, WindowNodeState::CanIdle);
        listNode.push_back(firstNode);
        
         // Create C2 nodes in a line
        for(int i = 1; i<5; i++){
            std::pair<int, int> coordinate = std::make_pair(800*i, 0);
            auto node = std::make_shared<C2_Node>(i, logger, coordinate,dispatchCv,dispatchCvMutex); // Create a smart pointer
            node->addActivation(baseTime+2000, WindowNodeState::CanListen); 
            node->addActivation(baseTime+4000, WindowNodeState::CanIdle);
            listNode.push_back(node);
        }


    }
