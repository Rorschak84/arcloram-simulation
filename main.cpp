#include "Node.hpp"
#include <thread>
#include "SimulationManager.hpp"
#include "Logger.hpp"
#include <atomic>
#include <list>
#include <conio.h> // For _kbhit() and _getch()
#include "C3/C3_Node.hpp"
#include "C2/C2_Node.hpp"
#include "C1/C1_Node.hpp"
#include "Clock.hpp"
#include "Seed.hpp"
#include "Common.hpp"




int main() {

//---------------------------------System Initialization---------------------------------
    //Logger
    Logger logger;
    logger.start();

    //Clock
     // Create a clock with a tick time of 5 milliseconds (it's actually a scheduler )
     //there is a tradeoff between the performance of the simulation and the representation of real signals that are in the order of milliseconds
     //TODO: change the name of CLock by Scheduler
    Clock clock(logger,100);//the tick interval should not be too small(<=100) otherwise the simulation has unpredicatable behavior (it's not an optimized scheduler I made here)
    //convert base time to milliseconds
    int64_t  baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() ;
    baseTime+=1000; //we add 4 seconds to the base time to allow the system to stabilize

    double distanceThreshold=1000;
    SimulationManager manager(distanceThreshold,logger);

//--------------------------------------------------------------Node Provisionning-------------------------------------------------

    
 

    Seed seed(std::string(communicationMode), std::string(topology),logger,manager.dispatchCv,manager.dispatchCvMutex,baseTime);
    
    auto nodes = seed.transferOwnership(); //the seed object memory is released safely
    manager.takeOwnership(std::move(nodes));




    //TODO: have a getter for the nodes list, or create a function, you should not allot the nodes to be accessible in public
    //MAKE THIS FUNCTION in the manager class? maybe not
    for(auto ptrNode : manager.nodes){
                // Schedule callbacks for the node's activations
        //we use the same callback that will call the correct calqlback Associated with the state transition
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
            //TODO: verify it's not destroying the performance, add a delay?
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
    logger.stop();//logger must outlive other objects since it's passed as a reference, otherwise unpredictable behavior

    return 0;
}
