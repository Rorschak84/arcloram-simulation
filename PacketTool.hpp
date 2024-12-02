#pragma once

#include <vector>
#include <string>
#include <cstdint>



// Function to get the current timestamp as a 4-byte value
std::vector<uint8_t> getTimeStamp();

std::string bytesToBinaryString(const std::vector<uint8_t>& packet);

uint32_t extractBytesFromField(const std::vector<uint8_t>& packet, const std::string& fieldName);


std::string bytesToDecimalString(const std::vector<uint8_t>& packet);


std::vector<uint8_t> decimalToBytes(uint32_t decimalValue, size_t byteCount);

std::string detailedBytesToString(const std::vector<uint8_t>& packet);

std::string bytesToHexString(const std::vector<uint8_t>& packet) ;

void addBitToPacket(std::vector<uint8_t>& packet, bool bit) ;

std::string bytesToHexString(const std::vector<uint8_t>& packet) ;

std::string bytesToBinary(const std::vector<uint8_t>& packet);

// Helper lambda to append vectors
inline auto appendVector = [](std::vector<uint8_t>& dest, const std::vector<uint8_t>& src) {
    dest.insert(dest.end(), src.begin(), src.end());
};