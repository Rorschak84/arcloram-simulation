#include "Node.hpp"
#include <thread>
#include "SimulationManager.hpp"
#include "Logger.hpp"
#include <atomic>
#include <conio.h> // For _kbhit() and _getch()

int main() {
//---------------------------------Initialization---------------------------------
    //Logger
    Logger logger;
    logger.start();

    //Node Seed
    int nbNodes = 4;
    SimulationManager manager(nbNodes);
    std::pair<int, int> coordinates = {0,0};
    for(int i = 0; i < manager.getNbNodes(); i++){
         auto node = std::make_shared<Node>(i, logger, coordinates); // Create a smart pointer
        manager.registerNode(node);
    }


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
    manager.stopSimulation();
    logger.logMessage("Simulation stopped.");
    logger.stop();
    // Wait for worker thread to finish
    worker.join();

    return 0;
}
