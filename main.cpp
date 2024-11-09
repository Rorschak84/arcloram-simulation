#include "Node.hpp"
#include <thread>
#include "SimulationManager.hpp"
#include "Logger.hpp"
#include <atomic>
#include <list>
#include <conio.h> // For _kbhit() and _getch()

int main() {

//---------------------------------Initialization---------------------------------
    //Logger
    Logger logger;
    logger.start();

    //------Node Seed---------
    int nbNodes = 4;
    double treshold = 1000;
    SimulationManager manager(nbNodes,treshold);
    //for now, we have a line topology, later we need to create a seed class (or a file) to define the topology
    std::vector<std::pair<int, int>> nodeCoordinates;
    for (int i = 0; i < nbNodes; ++i) {
         nodeCoordinates.emplace_back(i * 800, 0); // Add a pair (i * 800, 0)
    }
    

    // Create nodes and link to the manager
    for(int i = 0; i < manager.getNbNodes(); i++){
       
         auto node = std::make_shared<Node>(i, logger, nodeCoordinates[i]); // Create a smart pointer
        manager.registerNode(node);
    }

    //PHY Layer: based on the max distance 
    std::vector<std::vector<std::shared_ptr<Node>>> reachablesNodesPerNode = manager.getReachableNodesForAllNodes();
//---------------------------------Background---------------------------------

    std::atomic<bool> running(true);


    // Background thread that runs the simulation
    std::thread worker([&running,&manager]() {
         //start simulation
        manager.startSimulation();
        while (running) { }
    });



//---------------------------------Main---------------------------------

    //we want to stop the simulation when user clicks "q"
    while (running) {
        if (_kbhit()) { // Check if a key has been pressed
            char c = _getch(); // Get the character
            if (c == 'q') {
                running = false;
                logger.logMessage("q pressed, stopping simulation...");
            }
        }
    }


//---------------------------------End---------------------------------
    manager.stopTransmissionLoop();
    manager.stopSimulation();
    logger.logMessage("Simulation stopped.");
    logger.stop();
    // Wait for worker thread to finish
    worker.join();

    return 0;
}
