#pragma once


#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <fstream>
#include "TCP/Client.hpp"
#include <SFML/Network.hpp>

struct Log {
    std::string message;
    bool forTerminal;

    // Optional: Constructor for convenience
    Log(const std::string& msg, bool terminalOutput)
        : message(msg), forTerminal(terminalOutput) {

    }   

};

class Logger{

    public:
     void start();
     void stop();
     void logMessage(const Log& log);
     void sendTcpPacket(sf::Packet packet);
     std::string serverIp;
    unsigned int serverPort;

     Logger(std::string serverIp, unsigned int serverPort)
        :serverIp(serverIp), serverPort(serverPort){

        client = std::make_unique<Client>(serverIp, serverPort);

        };


    private:
    std::mutex queueMutex;//even though we have two queues, we need only one mutex to protect them
    std::condition_variable cv;

    std::queue<sf::Packet> tcpQueue;
    std::queue<Log> terminalQueue;
    std::thread loggerThread;
    bool stopFlag = false;
    // void flushBuffer(std::ofstream& logFile, std::vector<std::string>& buffer);
    std::unique_ptr<Client> client; // Pointer to the Client instance

    void processMessages();

};