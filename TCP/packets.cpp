

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
positionPacket::positionPacket( int id,int classNode,std::pair<int, int> coordinates,double batteryLevel)
    :nodeId(id),classNode(classNode), coordinates(std::move(coordinates)),batteryLevel(batteryLevel) {
    type = 3;
}

sf::Packet& operator<<(sf::Packet& packet, const positionPacket& pp) {
    return packet << pp.type << pp.nodeId<< pp.classNode << pp.coordinates.first << pp.coordinates.second<<pp.batteryLevel;
}

sf::Packet& operator>>(sf::Packet& packet, positionPacket& pp) {
    return packet >> pp.type >>pp.nodeId>>pp.classNode >> pp.coordinates.first >> pp.coordinates.second>>pp.batteryLevel;
}

// -------------------- transmitMessagePacket --------------------
transmitMessagePacket::transmitMessagePacket(int senderId, int receiverId,bool isACK)
    : senderId(senderId), receiverId(receiverId), isACK(isACK) {
    type = 4;
}

sf::Packet& operator<<(sf::Packet& packet, const transmitMessagePacket& tmp) {
    return packet << tmp.type << tmp.senderId << tmp.receiverId<<tmp.isACK;
}

sf::Packet& operator>>(sf::Packet& packet, transmitMessagePacket& tmp) {
    return packet >> tmp.type >> tmp.senderId >> tmp.receiverId>>tmp.isACK;
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

// -------------------- dropAnimationPacket --------------------
dropAnimationPacket::dropAnimationPacket(int nodeId) : nodeId(nodeId) {
    type = 8;
}
sf::Packet& operator<<(sf::Packet& packet, const dropAnimationPacket& dap) {
    return packet << dap.type << dap.nodeId;
}

sf::Packet& operator>>(sf::Packet& packet, dropAnimationPacket& dap) {
    return packet >> dap.type >> dap.nodeId;
}

// -------------------- retransmissionPacket --------------------
retransmissionPacket::retransmissionPacket(int nodeId) : nodeId(nodeId) {
    type = 9;
}

sf::Packet& operator<<(sf::Packet& packet, const retransmissionPacket& rp) {
    return packet << rp.type << rp.nodeId;
}
sf::Packet& operator>>(sf::Packet& packet, retransmissionPacket& rp) {
    return packet >> rp.type >> rp.nodeId;
}