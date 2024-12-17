#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <random>
#include <algorithm>
#include "Common.hpp"


// Function to get the current timestamp as a 4-byte value
std::vector<uint8_t> getTimeStamp();

std::string bytesToBinaryString(const std::vector<uint8_t>& packet);

uint32_t extractBytesFromField(const std::vector<uint8_t>& packet, const std::string& fieldName,const std::unordered_map<std::string, std::pair<size_t, size_t>> fieldMap);


std::string bytesToDecimalString(const std::vector<uint8_t>& packet);


std::vector<uint8_t> decimalToBytes(uint32_t decimalValue, size_t byteCount);

std::string detailedBytesToString(const std::vector<uint8_t>& packet, const std::unordered_map<std::string, std::pair<size_t, size_t>> fieldMap);

std::string bytesToHexString(const std::vector<uint8_t>& packet) ;

void addBitToPacket(std::vector<uint8_t>& packet, bool bit) ;

std::string bytesToHexString(const std::vector<uint8_t>& packet) ;

std::string bytesToBinary(const std::vector<uint8_t>& packet);

// Helper lambda to append vectors
inline auto appendVector = [](std::vector<uint8_t>& dest, const std::vector<uint8_t>& src) {
    dest.insert(dest.end(), src.begin(), src.end());
};


//----------------UTILITIES FOR RANDOMNESS-----------------


inline int computeRandomNbBeaconPackets(int min, int max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

inline  std::vector<int> selectRandomSlots(int m , int nbSlotsPossible) {
    
        // Step 1: Create a vector of slots [0, 1, ..., n-1]
        std::vector<int> slots(nbSlotsPossible);
        for (int i = 0; i < nbSlotsPossible; ++i) {
            slots[i] = i;
        }

        // Step 2: Shuffle the vector randomly
        std::random_device rd;  // Seed for random number generator
        std::mt19937 rng(rd()); // Mersenne Twister RNG
        std::shuffle(slots.begin(), slots.end(), rng);

        // Step 3: Take the first m slots
        std::vector<int> selected(slots.begin(), slots.begin() + m);
        
        // Step 4: Sort the selected slots in ascending order
        std::sort(selected.begin(), selected.end());

        return selected;
    }


inline std::vector<int> selectRandomOddSlots(int m, int nbSlotsPossible) {
    // Step 1: Create a vector of slots [0, 1, ..., n-1]
    std::vector<int> slots;
    for (int i = 0; i < nbSlotsPossible; ++i) {
        if (i % 2 != 0) { // Only odd slots
            slots.push_back(i);
        }
    }

    // Step 2: Shuffle the vector randomly
    std::random_device rd;  // Seed for random number generator
    std::mt19937 rng(rd()); // Mersenne Twister RNG
    std::shuffle(slots.begin(), slots.end(), rng);

    // Step 3: Select the first 'm' odd slots or fewer if not enough exist
    m = std::min(m, static_cast<int>(slots.size())); // Handle case where m > odd slots
    std::vector<int> selected(slots.begin(), slots.begin() + m);

    // Step 4: Sort the selected slots in ascending order
    std::sort(selected.begin(), selected.end());

    return selected;
}

inline std::vector<int> selectRandomEvenSlots(int m, int nbSlotsPossible) {
    // Step 1: Create a vector of slots [0, 1, ..., n-1]
    std::vector<int> slots;
    for (int i = 0; i < nbSlotsPossible; ++i) {
        if (i % 2 == 0) { // Only even slots
            slots.push_back(i);
        }
    }

    // Step 2: Shuffle the vector randomly
    std::random_device rd;  // Seed for random number generator
    std::mt19937 rng(rd()); // Mersenne Twister RNG
    std::shuffle(slots.begin(), slots.end(), rng);

    // Step 3: Select the first 'm' odd slots or fewer if not enough exist
    m = std::min(m, static_cast<int>(slots.size())); // Handle case where m > odd slots
    std::vector<int> selected(slots.begin(), slots.begin() + m);

    // Step 4: Sort the selected slots in ascending order
    std::sort(selected.begin(), selected.end());

    return selected;
}