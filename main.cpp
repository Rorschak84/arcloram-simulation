#include "SimulationManager.hpp"
#include <thread>


int main() {
    SimulationManager simManager;
    
    // Create nodes and register them with the manager
    Node node1(1, &simManager);
    Node node2(2, &simManager);
    simManager.registerNode(&node1);
    simManager.registerNode(&node2);
    
    // Launch nodes as threads
    std::thread t1(&Node::run, &node1);
    std::thread t2(&Node::run, &node2);
    
    // Start the simulation manager
    simManager.startSimulation();
    
    // Wait for node threads to complete
    t1.join();
    t2.join();
    
    return 0;
}
