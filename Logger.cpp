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

    void Logger::logMessage(const Log& log) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (log.forTerminal) {
            terminalQueue.push(log);
        } else {
            textQueue.push(log);
        }
        cv.notify_one();
    }




    void Logger::processMessages() {

        std::ofstream logFile("log.txt", std::ios::app); // Open log file in append mode
        if (!logFile) {
            std::cerr << "Failed to open log file!" << std::endl;
            return;
        }


        std::vector<std::string> textBuffer; // Buffer for batch processing
        std::vector<std::string> terminalBuffer; // Buffer for batch processing
        const size_t maxTerminalBufferSize = 1; // Set a buffer size limit
        const size_t maxTextBufferSize = 3; // Set a buffer size limit



        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex); //unique allows more flexibility (lock unlock) than lock_guard
            cv.wait(lock, //makes the thread wait until the condition variable cv is notified.
            [this] { return (!textQueue.empty()||!terminalQueue.empty()) || stopFlag; }); //ensures that the thread only proceeds if: one queue not empty OR stopflag is set
            
            //text messages--------------------------
            while (!textQueue.empty()) {


              textBuffer.push_back(textQueue.front().message);
                textQueue.pop();


            // Flush buffer if it reaches the limit
            if (textBuffer.size() >= maxTextBufferSize) {
                flushBuffer(logFile, textBuffer);
            }
            }

            //terminal messages--------------------

            while (!terminalQueue.empty()) {
                terminalBuffer.push_back(terminalQueue.front().message);
                terminalQueue.pop();

                // Flush buffer if it reaches the limit
                if (terminalBuffer.size() >= maxTerminalBufferSize) {
                    for (const auto& msg : terminalBuffer) {
                        std::cout << msg << std::endl;
                    }
                    terminalBuffer.clear(); // Clear the buffer after writing
                }
            }


            if (stopFlag) {
                flushBuffer(logFile, textBuffer); // Flush remaining messages
                break; // Exit the loop
            }
        }
    }

void Logger::flushBuffer(std::ofstream& logFile, std::vector<std::string>& buffer) {
    for (const auto& msg : buffer) {
        logFile << msg << std::endl;
    }
    buffer.clear(); // Clear the buffer after writing
}