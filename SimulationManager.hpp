#pragma once



#include <vector>
#include <mutex>
#include <condition_variable>
#include "Node.hpp" // Assuming Node is declared in Node.h

class SimulationManager {
public:
    SimulationManager(int nbNodes,double distanceThreshold);
    void startSimulation();
    void stopSimulation();
    void registerNode(std::shared_ptr<Node> node);
    int getNbNodes() const { return nbNodes; };

    // Function to get reachable nodes for a specific node
    std::vector<std::shared_ptr<Node>> getReachableNodesForNode(const std::shared_ptr<Node>& node);

    // Function to get reachable nodes for all nodes
    std::vector<std::vector<std::shared_ptr<Node>>> getReachableNodesForAllNodes();

private:
    int nbNodes;
    std::vector<std::shared_ptr<Node>> nodes;
    std::mutex terminalMutex;
    std::condition_variable cv;//TODO:for the logger, rename accordingly bc it's confusing
    int currentSlot = 0;
    bool running = true;

    double distanceThreshold;
    // Helper function to calculate Euclidean distance between two nodes
    double calculateDistance(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b);

};
