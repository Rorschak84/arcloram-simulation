#pragma once



#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Node.hpp" // Assuming Node is declared in Node.h

class SimulationManager {
public:
    SimulationManager(int nbNodes,double distanceThreshold);
    void startSimulation();
    void stopSimulation();
    void registerNode(std::shared_ptr<Node> node);
    int getNbNodes() const { return nbNodes; };


    // Starts the inter-node message dispatch loop using event-based triggering
    void startTransmissionLoop();
    void stopTransmissionLoop();

    // Function to get reachable nodes for a specific node
    std::vector<std::shared_ptr<Node>> getReachableNodesForNode(const std::shared_ptr<Node>& node);

    // Function to get reachable nodes for all nodes
    std::vector<std::vector<std::shared_ptr<Node>>> getReachableNodesForAllNodes();

private:
    int nbNodes;
    std::vector<std::shared_ptr<Node>> nodes;
    std::mutex terminalMutex;
     int currentSlot = 0;
    

    double distanceThreshold;
    // Helper function to calculate Euclidean distance between two nodes
    double calculateDistance(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b);



    std::condition_variable dispatchCv; // Condition variable for event-based triggering
    std::mutex dispatchCvMutex;         // Mutex associated with the condition variable

    void transmissionLoop(); // Main loop for handling transmissions
     std::atomic<bool> dispatchRunning;
     std::thread dispatchThread;
};
