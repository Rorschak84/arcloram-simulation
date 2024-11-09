#pragma once



#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Logger.hpp"


class Node {
public:
    Node(int id, Logger& logger);
    void run();
    void stop();

private:
    int nodeId;
    bool running;
    Logger& logger;
};

