#pragma once



#include <vector>
#include <mutex>
#include <condition_variable>
#include "Node.hpp" // Assuming Node is declared in Node.h

class SimulationManager {
public:
    SimulationManager(int nbNodes);
    void startSimulation();
    void stopSimulation();
    void registerNode(std::shared_ptr<Node> node);
    int getNbNodes() const { return nbNodes; };

    void printMessage(const std::string& message);


private:
    int nbNodes;


    std::vector<std::shared_ptr<Node>> nodes;
    std::mutex terminalMutex;
    std::condition_variable cv;
    int currentSlot = 0;
    bool running = true;
};
