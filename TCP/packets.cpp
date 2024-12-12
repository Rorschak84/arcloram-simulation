

#include "packets.hpp"

// -------------------- systemPacket --------------------
systemPacket::systemPacket(double distanceThreshold, std::string mode)
    : distanceThreshold(distanceThreshold), mode(std::move(mode)) {
    type = 0;
}

sf::Packet& operator<<(sf::Packet& packet, const systemPacket& sp) {
    return packet << sp.type << sp.distanceThreshold << sp.mode;
}

sf::Packet& operator>>(sf::Packet& packet, systemPacket& sp) {
    return packet >> sp.type >> sp.distanceThreshold >> sp.mode;
}

// -------------------- tickPacket --------------------
tickPacket::tickPacket(int tickNb) : tickNb(tickNb) {
    type = 1;
}

sf::Packet& operator<<(sf::Packet& packet, const tickPacket& tp) {
    return packet << tp.type << tp.tickNb;
}

sf::Packet& operator>>(sf::Packet& packet, tickPacket& tp) {
    return packet >> tp.type >> tp.tickNb;
}

// -------------------- stateNodePacket --------------------
stateNodePacket::stateNodePacket(int nodeId, std::string state)
    : nodeId(nodeId), state(std::move(state)) {
    type = 2;
}

sf::Packet& operator<<(sf::Packet& packet, const stateNodePacket& snp) {
    return packet << snp.type << snp.nodeId << snp.state;
}

sf::Packet& operator>>(sf::Packet& packet, stateNodePacket& snp) {
    return packet >> snp.type >> snp.nodeId >> snp.state;
}

// -------------------- positionPacket --------------------
positionPacket::positionPacket( int id,int classNode,std::pair<int, int> coordinates)
    :nodeId(id),classNode(classNode), coordinates(std::move(coordinates)) {
    type = 3;
}

sf::Packet& operator<<(sf::Packet& packet, const positionPacket& pp) {
    return packet << pp.type << pp.nodeId<< pp.classNode << pp.coordinates.first << pp.coordinates.second;
}

sf::Packet& operator>>(sf::Packet& packet, positionPacket& pp) {
    return packet >> pp.type >>pp.nodeId>>pp.classNode >> pp.coordinates.first >> pp.coordinates.second;
}

// -------------------- transmitMessagePacket --------------------
transmitMessagePacket::transmitMessagePacket(int senderId, int receiverId)
    : senderId(senderId), receiverId(receiverId) {
    type = 4;
}

sf::Packet& operator<<(sf::Packet& packet, const transmitMessagePacket& tmp) {
    return packet << tmp.type << tmp.senderId << tmp.receiverId;
}

sf::Packet& operator>>(sf::Packet& packet, transmitMessagePacket& tmp) {
    return packet >> tmp.type >> tmp.senderId >> tmp.receiverId;
}

// -------------------- receiveMessagePacket --------------------
receiveMessagePacket::receiveMessagePacket(int senderId, int receiverId, std::string state)
    : senderId(senderId), receiverId(receiverId), state(std::move(state)) {
    type = 5;
}

sf::Packet& operator<<(sf::Packet& packet, const receiveMessagePacket& rmp) {
    return packet << rmp.type << rmp.senderId << rmp.receiverId << rmp.state;
}

sf::Packet& operator>>(sf::Packet& packet, receiveMessagePacket& rmp) {
    return packet >> rmp.type >> rmp.senderId >> rmp.receiverId >> rmp.state;
}

// -------------------- routingDecisionPacket --------------------
routingDecisionPacket::routingDecisionPacket(int receiverId, int senderId, bool newRoute)
    : receiverId(receiverId), senderId(senderId), newRoute(newRoute) {
    type = 6;
}

sf::Packet& operator<<(sf::Packet& packet, const routingDecisionPacket& rdp) {
    return packet << rdp.type << rdp.receiverId << rdp.senderId << rdp.newRoute;
}

sf::Packet& operator>>(sf::Packet& packet, routingDecisionPacket& rdp) {
    return packet >> rdp.type >> rdp.receiverId >> rdp.senderId >> rdp.newRoute;
}

// -------------------- broadcastMessagePacket --------------------
broadcastMessagePacket::broadcastMessagePacket(int nodeId) : nodeId(nodeId) {
    type = 7;
}

sf::Packet& operator<<(sf::Packet& packet, const broadcastMessagePacket& bmp) {
    return packet << bmp.type << bmp.nodeId;
}

sf::Packet& operator>>(sf::Packet& packet, broadcastMessagePacket& bmp) {
    return packet >> bmp.type >> bmp.nodeId;
}
