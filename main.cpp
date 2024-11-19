#include "Node.hpp"
#include <thread>
#include "SimulationManager.hpp"
#include "Logger.hpp"
#include <atomic>
#include <list>
#include <conio.h> // For _kbhit() and _getch()
#include "C3_Node.hpp"
#include "C2_Node.hpp"
#include "C1_Node.hpp"
#include "Clock.hpp"

int main() {

//---------------------------------Initialization---------------------------------
    //Logger
    Logger logger;
    logger.start();

    //Clock
     // Create a clock with a tick time of 10 milliseconds (it's actually a scheduler )
     //TODO: change the name of CLock by Scheduler
    Clock clock(logger,10);
    //convert base time to milliseconds
    int64_t  baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() ;

    //------Node Seed---------

    //BE extra careful about the order of the nodes in the vector, it will be used to determine the id of the nodes

    int nbNodes = 4;
    double treshHold = 1000;//for the max distance of communication
    SimulationManager manager(nbNodes,treshHold,logger);

    
    //for now, we have a line topology, later we need to create a seed class (or a file) to define the topology
    std::vector<std::pair<int, int>> nodeCoordinates;
    for (int i = 0; i < nbNodes; ++i) {
         nodeCoordinates.emplace_back(i * 800, 0); // Add a pair (i * 800, 0)
    }
    
    //create a C3 node
    //there should be a little offset in the beginning to allow the system to stabilize
    auto firstNode = std::make_shared<C3_Node>(0, logger, nodeCoordinates[0],manager.dispatchCv,manager.dispatchCvMutex);
    firstNode->addActivation(baseTime+200, WindowNodeState::CanTransmit); 
    firstNode->addActivation(baseTime+300, WindowNodeState::CanIdle);
    manager.registerNode(firstNode);

    Log timelog("TimeLOG:"+std::to_string(baseTime+1100)+"    "+std::to_string(baseTime+2100), true);
    logger.logMessage(timelog);

    // // Create nodes and link to the manager
    // for(int i = 1; i < manager.getNbNodes(); i++){
       
    //      auto node = std::make_shared<C2_Node>(i, logger, nodeCoordinates[i],manager.dispatchCv,manager.dispatchCvMutex); // Create a smart pointer
    //     manager.registerNode(node);
    // }

    //TODO: have a getter for the nodes list, or create a function, you should not allot the nodes to be accessible in public
    for(auto ptrNode : manager.nodes){
                // Schedule callbacks for the node's activations
        //we use the same callback that will call the correct callback Associated with the state transition
        for (const auto& [activationTime, windowNodeState] : ptrNode->getActivationSchedule()) {
            clock.scheduleCallback(activationTime, [ptrNode,windowNodeState]() {
                ptrNode->onTimeChange(windowNodeState);//onTimeChange will call the callback associated with the proposed state and the currentState stored in the stateTransitions variable
            });
        }
    }

  
//---------------------------------Background---------------------------------

    std::atomic<bool> running(true);

    Log startingLog("Starting Simulation...", true);
    logger.logMessage(startingLog);


    
    std::thread worker([&running, &manager,&clock]() {
        // Start simulation
        manager.startSimulation();
        // Background thread that runs the simulation
         clock.start();
        while (running) {
            //TODO: verify it's not destroying the performance
        }
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
    clock.stop();
    Log clockstopLog("Scheduler stopped...", true);
    logger.logMessage(clockstopLog);
    manager.stopTransmissionLoop();
     Log stoppingLog("transmission loop stopped...", true);
    logger.logMessage(stoppingLog);
    manager.stopSimulation();
    Log stoppingLog2("Simulation Manager stopped...", true);
    logger.logMessage(stoppingLog2);
    // Wait for worker thread to finish
    worker.join();
    Log stoppedLog("Simulation Stopped... Thank you for using ArcLoRaM Simulator", true);
    logger.logMessage(stoppedLog);
    logger.stop();

    return 0;
}
