#include "Logger.hpp"


    void Logger::start() {
        loggerThread = std::thread(&Logger::processMessages, this);
    }

    void Logger::stop() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        cv.notify_all();
        if (loggerThread.joinable()) {
            loggerThread.join();
        }
    }

    void Logger::logMessage(const std::string& message) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push(message);
        }
        cv.notify_one(); // Notify the logger thread
    }

    void Logger::processMessages() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex); //unique allows more flexibility (lock unlock) than lock_guard
            cv.wait(lock, //makes the thread wait until the condition variable cv is notified.
            [this] { return !messageQueue.empty() || stopFlag; }); //ensures that the thread only proceeds if: queue not empty OR stopflag is set
            while (!messageQueue.empty()) {
                std::cout << messageQueue.front() << std::endl;
                messageQueue.pop();
            }
            if (stopFlag) break;
        }
    }
