#include <iosfwd>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "helpFunctions.hpp"

// Function to read numerous bytes
unsigned long long readLittleEndian(std::fstream& file, const int NumberBytesToRead, const EndianReadType type) {
    if (!file.is_open()) {
        std::cerr << "Error: opening file" << std::endl;
        return 0;
    }
    if (NumberBytesToRead <= 0 || NumberBytesToRead > 8) {
        std::cerr << "Error: Invalid number of bytes specified for reading (" << NumberBytesToRead << ")." << std::endl;
        return 0;
    }
    std::vector<unsigned char> buffer(NumberBytesToRead);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), NumberBytesToRead)) {
        std::cerr << "Error: reading " << NumberBytesToRead << " bytes from file." << std::endl;
        return 0;
    }

    unsigned long long value = 0;
    // Build the value from bytes in little-endian order
    for (int i = 0; i < NumberBytesToRead; ++i) {
        value |= static_cast<unsigned long long>(buffer[i]) << (i * 8);
    }

    // Interpret the value based on the requested type
    switch (type) {
        case EndianReadType::USHORT: // lower 16 bits are relevant
        case EndianReadType::UINT:
            return value; // lower 32 bits are relevant
        case EndianReadType::INT:
            return static_cast<unsigned long long>(static_cast<int>(value));
        default:
            std::cerr << "Error: Unknown EndianReadType." << std::endl;
        return 0;
    }
}

std::vector<bool> textToBits(std::string& secretMessageInText) {
    secretMessageInText.push_back('\0');
    std::vector<bool> secretMessageInBit;
    for (char letter : secretMessageInText) {
        for (int j = 7 ; j >= 0 ; --j) {
            bool bit = (letter >> j) & 1;
            secretMessageInBit.push_back(bit);
        }
    }
    return secretMessageInBit;
}
std::string bitsToText(const std::vector<bool>& secretMessageInBit) {
    std::string secretMessageInText;
    char letter = 0;
    int bitCount = 0;
    for (bool bit : secretMessageInBit ) {
        letter <<= 1;
        if (bit){ letter |= 1;}
        bitCount++;
        if (bitCount == 8) {
            secretMessageInText.push_back(letter);
            if (letter == '\0') {
                return secretMessageInText;
            }
            letter = 0;
            bitCount = 0;
        }
    }
    return secretMessageInText;
}
