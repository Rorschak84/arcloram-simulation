#ifndef NODE_H
#define NODE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "SimulationManager.hpp" // Assuming mutual dependency

class Logger; // Forward declaration

class Node {
public:
    Node(int id, SimulationManager* simManager);
    void run();
    void receiveSignal(Message msg);
    void logEvent(const std::string& event);

private:
    int nodeId;
    SimulationManager* simManager;
    std::shared_ptr<Logger> logger;
    std::queue<Message> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
};

#endif // NODE_H
