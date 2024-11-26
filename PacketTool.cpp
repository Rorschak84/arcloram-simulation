#include "PacketTool.hpp"
#include <sstream>
#include <iomanip>

//-----TO add a byte
// packet.push_back(0x2C) for instance

//----- Add multiple bytes
// std::vector<uint8_t> payload = {0x02, 0x03, 0x04};
// packet.insert(packet.end(), payload.begin(), payload.end());


//----Extract Bytes:
// Extract the first two bytes as a 16-bit integer
// uint16_t header = (packet[0] << 8) | packet[1];
// std::cout << std::hex << header << "\n"; // Output: AABB

// Extract a payload starting from the 3rd byte
// std::vector<uint8_t> payload(packet.begin() + 2, packet.end());


//----Spliting a frame:
// Split at index 3
// size_t split_index = 3;
// std::vector<uint8_t> first_part(frame.begin(), frame.begin() + split_index);
// std::vector<uint8_t> second_part(frame.begin() + split_index, frame.end());

//----Combining packets:
// packet1.insert(packet1.end(), packet2.begin(), packet2.end());

//----Removing Bytes
// Remove the second byte
//packet.erase(packet.begin() + 1);

// Remove a range (e.g., from index 1 to 2)
//packet.erase(packet.begin() + 1, packet.end());


//---- Inserting Bytes:
// Insert two bytes at position 1
//packet.insert(packet.begin() + 1, {0x02, 0x03});




std::string packet_to_binary(const std::vector<uint8_t>& packet) {
    std::ostringstream oss;
    for (auto byte : packet) {
        // Convert each byte to its binary representation
        for (int i = 7; i >= 0; --i) {
            oss << ((byte >> i) & 1);
        }
        oss << " "; // Space between bytes
    }
    return oss.str();
}

std::string packet_to_hex(const std::vector<uint8_t>& packet) {
    std::ostringstream oss;
    for (auto byte : packet) {
        // Convert each byte to a two-digit hexadecimal number
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    return oss.str();
}


void add_bit_to_packet(std::vector<uint8_t>& packet, bool bit) {
    static uint8_t current_byte = 0;
    static int bit_count = 0;

    // Add the bit to the current byte
    current_byte |= (bit << (7 - bit_count));
    bit_count++;

    // If the byte is full, add it to the packet and reset
    if (bit_count == 8) {
        packet.push_back(current_byte);
        current_byte = 0;
        bit_count = 0;
    }
}