#include "PacketTool.hpp"
#include <sstream>
#include <iomanip>
#include "Common.hpp"
#include <unordered_map>
#include <algorithm>



//-----------------------------------------------vector<int8_t> TRICKS--------------------------------------
//-----TO add a byte
// packet.push_back(0x2C) for instance

//----- Add multiple bytes
// std::vector<uint8_t> payload = {0x02, 0x03, 0x04};
// packet.insert(packet.end(), payload.begin(), payload.end());


//----Extract Bytes: USE MASKS
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


//-----------------------------------------------END--------------------------------------


// Function to extract a field's value
//IF we need to process the Payload with this, since it can be 40 bytes maximum, we need to use a uint64_t (or maybe higher?)
//unless we don't want to print the payload
uint32_t extractBytesFromField(const std::vector<uint8_t>& packet, const std::string& fieldName) {
    // Check if the field name exists in the map
    
    auto it = common::fieldMap.find(fieldName);
    if (it == common::fieldMap.end()) {
        throw std::invalid_argument("Field name not recognized: " + fieldName);
    }

    // Extract start index and size
    size_t startIndex = it->second.first;
    size_t size = it->second.second;

    // Ensure the packet has enough data
    if (startIndex + size > packet.size()) {
        throw std::out_of_range("Packet size is smaller than expected for field: " + fieldName);
    }

    // Compute the field's decimal value (little-endian interpretation)
    uint32_t value = 0;
    for (size_t i = 0; i < size; ++i) {
        value |= packet[startIndex + i] << (8 * i); // Combine bytes in little-endian order
    }

    return value;
}

// Function to convert a packet of bytes to a space-separated decimal string
inline std::string bytesToDecimal(const std::vector<uint8_t>& packet) {
    std::ostringstream oss;
    for (auto byte : packet) {
        oss << static_cast<int>(byte) << " "; // Convert each byte to decimal
    }
    std::string result = oss.str();
    if (!result.empty()) {
        result.pop_back(); // Remove the trailing space
    }
    return result;
}

// Function to convert a decimal value into a std::vector<uint8_t>
inline std::vector<uint8_t> decimalToBytes(uint32_t decimalValue, size_t byteCount) {
    std::vector<uint8_t> bytes(byteCount, 0);

    for (size_t i = 0; i < byteCount; ++i) {
        // Extract the least significant byte and store it in reverse order
        bytes[byteCount - 1 - i] = static_cast<uint8_t>(decimalValue & 0xFF);
        decimalValue >>= 8; // Shift the value to process the next byte
    }

    return bytes;
}


// Function to generate the detailed string representation of the packet
std::string detailedBytesToString(const std::vector<uint8_t>& packet) {
    std::ostringstream oss;
     // Collect keys in reverse order
    std::vector<std::string> fieldNames;
    for (const auto& [fieldName, fieldInfo] : common::fieldMap) {
        fieldNames.push_back(fieldName);
    }

    // Reverse the field names
    std::reverse(fieldNames.begin(), fieldNames.end());

    // Process the fields in reverse order
    for (const auto& fieldName : fieldNames) {
        uint32_t value = extractBytesFromField(packet, fieldName);
        oss << fieldName << ": " << value << " - ";
    }

    std::string result = oss.str();
    if (!result.empty()) {
        result.erase(result.size() - 3); // Remove the last trailing " - "
    }

    return result;
}

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







//not used, we only care of bytes
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

std::string bytesToHex(const std::vector<uint8_t>& packet) {
    std::ostringstream oss;
    for (auto byte : packet) {
        // Convert each byte to a two-digit hexadecimal number
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    return oss.str();
}


std::string bytesToBinary(const std::vector<uint8_t>& packet) {
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