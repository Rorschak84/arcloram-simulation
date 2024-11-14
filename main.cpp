#include "Node.hpp"
#include <thread>
#include "SimulationManager.hpp"
#include "Logger.hpp"
#include <atomic>
#include <list>
#include <conio.h> // For _kbhit() and _getch()
#include "Log.cpp"
#include "C3_Node.hpp"
#include "C2_Node.hpp"
#include "C1_Node.hpp"

int main() {

//---------------------------------Initialization---------------------------------
    //Logger
    Logger logger;
    logger.start();

    //------Node Seed---------

    //BE extra careful about the order of the nodes in the vector, it will be used to determine the id of the nodes

    int nbNodes = 50;
    double treshold = 1000;
    SimulationManager manager(nbNodes,treshold,logger);
    //for now, we have a line topology, later we need to create a seed class (or a file) to define the topology
    std::vector<std::pair<int, int>> nodeCoordinates;
    for (int i = 0; i < nbNodes; ++i) {
         nodeCoordinates.emplace_back(i * 800, 0); // Add a pair (i * 800, 0)
    }
    
    //create a C3 node
    auto firstNode = std::make_shared<C3_Node>(0, logger, nodeCoordinates[0],manager.dispatchCv,manager.dispatchCvMutex); 
    manager.registerNode(firstNode);


    // Create nodes and link to the manager
    for(int i = 1; i < manager.getNbNodes(); i++){
       
         auto node = std::make_shared<C2_Node>(i, logger, nodeCoordinates[i],manager.dispatchCv,manager.dispatchCvMutex); // Create a smart pointer
        manager.registerNode(node);
    }

  
//---------------------------------Background---------------------------------

    std::atomic<bool> running(true);

    Log startingLog("Starting Simulation...", true);
    logger.logMessage(startingLog);
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
                Log stoppingLog("q pressed, stopping simulation...", true);
                logger.logMessage(stoppingLog);
            }
        }
    }


//---------------------------------End---------------------------------
    manager.stopTransmissionLoop();
    manager.stopSimulation();
    Log stoppedLog("Simulation Stopped...", true);
    logger.logMessage(stoppedLog);
    logger.stop();
    // Wait for worker thread to finish
    worker.join();

    return 0;
}
