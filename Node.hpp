#pragma once



#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>


class SimulationManager;


class Node {
public:
    Node(int id);
    void run();
    void stop();

private:
    int nodeId;
    bool running;
    SimulationManager* manager;
   
};

