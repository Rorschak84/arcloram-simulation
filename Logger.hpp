#pragma once


#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <fstream>


struct Log {
    std::string message;
    bool forTerminal;

    // Optional: Constructor for convenience
    Log(const std::string& msg, bool terminalOutput)
        : message(msg), forTerminal(terminalOutput) {}
};

class Logger{

    public:
     void start();
     void stop();
     void logMessage(const Log& log);

     private:
    std::mutex queueMutex;//even though we have two queues, we need only one mutex to protect them
    std::condition_variable cv;

    std::queue<Log> textQueue;
    std::queue<Log> terminalQueue;
    std::thread loggerThread;
    bool stopFlag = false;
    void flushBuffer(std::ofstream& logFile, std::vector<std::string>& buffer);

    void processMessages();
};