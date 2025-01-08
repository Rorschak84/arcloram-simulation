


#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <SFML/Network.hpp>
#include <string>
#include <utility>

// Base class for packet type identification
class BasePacket {
public:
    int type;
    virtual ~BasePacket() = default; // Virtual destructor for polymorphism
};

// Packet class declarations
class systemPacket : public BasePacket {
public:
    double distanceThreshold;
    std::string mode;

    systemPacket(double distanceThreshold = 100, std::string mode = "error");
    friend sf::Packet& operator<<(sf::Packet& packet, const systemPacket& sp);
    friend sf::Packet& operator>>(sf::Packet& packet, systemPacket& sp);
};

class tickPacket : public BasePacket {
public:
    int tickNb;

    tickPacket(int tickNb = 0);
    friend sf::Packet& operator<<(sf::Packet& packet, const tickPacket& tp);
    friend sf::Packet& operator>>(sf::Packet& packet, tickPacket& tp);
};

class stateNodePacket : public BasePacket {
public:
    int nodeId;
    std::string state;

    stateNodePacket(int nodeId = 0, std::string state = "error");
    friend sf::Packet& operator<<(sf::Packet& packet, const stateNodePacket& snp);
    friend sf::Packet& operator>>(sf::Packet& packet, stateNodePacket& snp);
};


//should be renamed initialNodePacket
class positionPacket : public BasePacket {
public:
    int nodeId;
    std::pair<int, int> coordinates;
    int classNode;
    double batteryLevel;
    positionPacket(int id=0, int classNode =0,std::pair<int, int> coordinates = { 0, 0 }, double batteryLevel=0.0);
    friend sf::Packet& operator<<(sf::Packet& packet, const positionPacket& pp);
    friend sf::Packet& operator>>(sf::Packet& packet, positionPacket& pp);
};

class transmitMessagePacket : public BasePacket {
public:
    int senderId;
    int receiverId;
    bool isACK;

    transmitMessagePacket(int senderId = 0, int receiverId = 0,bool isACK=false);
    friend sf::Packet& operator<<(sf::Packet& packet, const transmitMessagePacket& tmp);
    friend sf::Packet& operator>>(sf::Packet& packet, transmitMessagePacket& tmp);
};

class receiveMessagePacket : public BasePacket {
public:
    int senderId;
    int receiverId;
    std::string state;

    receiveMessagePacket(int senderId = 0, int receiverId = 0, std::string state = "error");
    friend sf::Packet& operator<<(sf::Packet& packet, const receiveMessagePacket& rmp);
    friend sf::Packet& operator>>(sf::Packet& packet, receiveMessagePacket& rmp);
};

class routingDecisionPacket : public BasePacket {
public:
    int receiverId;
    int senderId;
    bool newRoute;

    routingDecisionPacket(int receiverId = 0, int senderId = 0, bool newRoute = false);
    friend sf::Packet& operator<<(sf::Packet& packet, const routingDecisionPacket& rdp);
    friend sf::Packet& operator>>(sf::Packet& packet, routingDecisionPacket& rdp);
};

class broadcastMessagePacket : public BasePacket {
public:
    int nodeId;

    broadcastMessagePacket(int nodeId = 0);
    friend sf::Packet& operator<<(sf::Packet& packet, const broadcastMessagePacket& bmp);
    friend sf::Packet& operator>>(sf::Packet& packet, broadcastMessagePacket& bmp);
};


class dropAnimationPacket : public BasePacket {
public:
    int nodeId;

    dropAnimationPacket(int nodeId = 0);
    friend sf::Packet& operator<<(sf::Packet& packet, const dropAnimationPacket& bmp);
    friend sf::Packet& operator>>(sf::Packet& packet, dropAnimationPacket& bmp);
};

class retransmissionPacket : public BasePacket {
    public:
    int nodeId;
    retransmissionPacket(int nodeId = 0);
    friend sf::Packet& operator<<(sf::Packet& packet, const retransmissionPacket& rp);
    friend sf::Packet& operator>>(sf::Packet& packet, retransmissionPacket& rp);
};

#endif // PACKETS_HPP
