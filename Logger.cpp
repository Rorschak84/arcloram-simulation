#include "Logger.hpp"
#include "Common.hpp"


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
        } 
        cv.notify_one();
    }

    void Logger::sendTcpPacket(sf::Packet packet){
       
        if(!common::visualiserConnected) return;
        
        std::lock_guard<std::mutex> lock(queueMutex);

        if(packet){
            tcpQueue.push(packet);
            cv.notify_one();
        }
            
    }




    void Logger::processMessages() {

        // std::ofstream logFile("log.txt", std::ios::app); // Open log file in append mode
        // if (!logFile) {
        //     std::cerr << "Failed to open log file!" << std::endl;
        //     return;
        // }


        // std::vector<std::string> textBuffer; // Buffer for batch processing
        std::vector<std::string> terminalBuffer; // Buffer for batch processing
        const size_t maxTerminalBufferSize = 5; // Set a buffer size limit
        const size_t maxTextBufferSize = 3; // Set a buffer size limit



        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex); //unique allows more flexibility (lock unlock) than lock_guard
            cv.wait(lock, //makes the thread wait until the condition variable cv is notified.
            [this] { return (!tcpQueue.empty()||!terminalQueue.empty()) || stopFlag; }); //ensures that the thread only proceeds if: one queue not empty OR stopflag is set
            
            //tcp packets--------------------------
            while (!tcpQueue.empty()) {

                if(client->transmit(tcpQueue.front())){
                    tcpQueue.pop();
                }
                 else{
                    
                    if(common::visualiserConnected){
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        //and we retry
                    }
                    else{
                        break;
                    }


                 }
            }

            //terminal messages--------------------

            while (!terminalQueue.empty()) {
                terminalBuffer.push_back(terminalQueue.front().message);
                terminalQueue.pop();

                // Flush buffer if it reaches the limit
                if (terminalBuffer.size() >= maxTerminalBufferSize) {
                    for (const auto& msg : terminalBuffer) {
                        std::cout << msg << "\n"; //"\n" and endl is not the same !
                    }
                    std::cout<<std::flush;
                    terminalBuffer.clear(); // Clear the buffer after writing
                }
            }


            if (stopFlag) {
                for (const auto& msg : terminalBuffer) {
                    std::cout << msg << "\n"; //"\n" and endl is not the same !
                }
                std::cout<<std::flush;               
                // flushBuffer(logFile, textBuffer); // Flush remaining messages
                break; // Exit the loop
            }
        }
    }

