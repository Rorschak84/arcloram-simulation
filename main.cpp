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
#include "TCP/packets.hpp"




int main() {

    sf::TcpListener listener;

// sf::TcpSocket socket;

//---------------------------------System Initialization---------------------------------
    //Logger
    Logger logger("127.0.0.1",5000);
    logger.start();


    //visualiser configuration
    sf::Packet sysPacketReceiver;
    systemPacket sysPacket(common::distanceThreshold, common::communicationMode);
    sysPacketReceiver<<sysPacket;
    logger.sendTcpPacket(sysPacketReceiver);

    //Clock

    Clock clock(logger,common::tickIntervalForClock_ms);//the tick interval should not be too small(<=100) otherwise the simulation has unpredicatable behavior (it's not an optimized scheduler I made here)
    //convert base time to milliseconds
    int64_t  baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() ;
    baseTime+=common::baseTimeOffset; //allow the system to initialize before the TDMA begins

    
    SimulationManager manager(common::distanceThreshold,logger);

//--------------------------------------------------------------Node Provisionning-------------------------------------------------

    
 

    Seed seed(std::string(common::communicationMode), std::string(common::topology),logger,manager.dispatchCv,manager.dispatchCvMutex,baseTime);
    
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
            std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Avoid busy-waiting
        }
      }
    );



//---------------------------------Main---------------------------------

    //we want to stop the simulation when user clicks "q"
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Avoid busy-waiting
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
