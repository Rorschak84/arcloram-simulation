#pragma once


#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>


class Logger{

    public:
     void start();
     void stop();
     void logMessage(const std::string& message);

     private:
    std::mutex queueMutex;
    std::condition_variable cv;
    std::queue<std::string> messageQueue;
    std::thread loggerThread;
    bool stopFlag = false;

    void processMessages();
};