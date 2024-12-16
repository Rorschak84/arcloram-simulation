#include "Client.hpp"
#include <thread>
#include <chrono>
#include "../Common.hpp"

Client::Client(const std::string& serverIp, unsigned short serverPort)
    : isConnected(false) {
    if(!common::visualiserConnected){
        std::cout << "****** No Visualiser Mode ******\n";
        return;
    }
    // Attempt to connect to the server
    if (socket.connect(serverIp, serverPort) != sf::Socket::Done) {
        std::cerr << "******Error connecting to server******\n";
        return;
    }

    std::cout << "******Connected to server!******\n";
    isConnected = true;

}

Client::~Client() {
    if (isConnected) {
        socket.disconnect();
        std::cout << "******Disconnected from server.******\n";
    }
}

bool Client::transmit(sf::Packet& packet) {
    if (!isConnected) {
        std::cerr << "******Client is not connected to a server.******\n";
        return false;
    }

    while (true) {
        sf::Socket::Status status = socket.send(packet);
        if (status == sf::Socket::Done) {
            //std::cout << "******Packet sent successfully.******\n";
            return true;
        } else if (status == sf::Socket::Partial) {
           // std::cout << "******Partial send, retrying...******\n";
        } else {
            std::cerr << "******Error sending packet.******\n";
            return false;
        }
    }
}

bool Client::isConnectedToServer() const {
    return isConnected;
}
