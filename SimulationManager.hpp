#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include "Node.h" // Assuming Node is declared in Node.h

class SimulationManager {
public:
    SimulationManager();
    void startSimulation();
    void registerNode(Node* node);
    void transmitSignal(Node* sender, Message msg);
    void tdmaSlotControl();

private:
    std::vector<Node*> nodes;
    std::mutex managerMutex;
    std::condition_variable cv;
    int currentSlot = 0;
    bool running = true;
};

#endif // SIMULATION_MANAGER_H
