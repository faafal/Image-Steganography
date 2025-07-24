#ifndef HELPFUNCTIONS_HPP
#define HELPFUNCTIONS_HPP

enum class EndianReadType {
    USHORT,
    UINT,
    INT
};

unsigned long long readLittleEndian(std::fstream& file, int NumberBytesToRead, EndianReadType type);

std::vector<bool> textToBits(std::string& secretMessageInText);
std::string bitsToText(const std::vector<bool>& secretMessageInBit);

#endif //HELPFUNCTIONS_HPP
