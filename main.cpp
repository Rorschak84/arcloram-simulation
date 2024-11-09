#include "Node.hpp"
#include <thread>
#include "SimulationManager.hpp"
#include "Logger.hpp"

int main() {

    Logger logger;
    logger.start();


    int nbNodes = 1;
    SimulationManager manager(nbNodes);
    
    for(int i = 0; i < manager.getNbNodes(); i++){
         auto node = std::make_shared<Node>(i); // Create a smart pointer
        manager.registerNode(node);
    }
    manager.startSimulation();

    //need to change this
    std::this_thread::sleep_for(std::chrono::seconds(5));


    manager.stopSimulation();


    logger.stop();
    return 0;
}
