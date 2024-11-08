#include "SimulationManager.hpp"
#include <iostream>

SimulationManager::SimulationManager() {
    // Constructor implementation
}

void SimulationManager::startSimulation() {
    // Implementation for starting the simulation
}

void SimulationManager::registerNode(Node* node) {
    std::lock_guard<std::mutex> lock(managerMutex);
    nodes.push_back(node);
}

void SimulationManager::transmitSignal(Node* sender, Message msg) {
    // Implementation for transmitting a signal
}

void SimulationManager::tdmaSlotControl() {
    // Implementation for TDMA slot control logic
}
