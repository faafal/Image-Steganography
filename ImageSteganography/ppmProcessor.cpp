#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "ppmProcessor.hpp"
#include "helpFunctions.hpp"

ppmObject::ppmObject(const std::string& inputFilePath) {
    filePath = inputFilePath;
}
bool ppmObject::isHeaderCorrect() {
    std::fstream file(filePath,std::ios::in);

    file >> magicNumber;
    if (magicNumber != "P6" && magicNumber != "P3") {
        std::cerr << "Error: File signature is incorrect."<< std::endl;;
        return false;
    }
    std::string token;
    int counter = 0;

    while (counter < 3 && file >> token) {
        if (token.empty()) {
            std::cerr << "Error: Unexpected empty token in header." << std::endl;
            return false;
        }
        if (token[0] == '#') {
            std::string discard;
            std::getline(file, discard);
            continue;
        }
        try {
            int value = std::stoi(token);
            switch (counter) {
                case 0: width = value; break;
                case 1: height = value; break;
                case 2: maxChannelValue = value; break;
                default: return 1;
            }
            counter++;
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid number in header (" << token << ")." << std::endl;
            return false;
        }
    }
    offset = counter;

    if (counter < 3) {
        std::cerr << "Error: Incomplete header." << std::endl;
        return false;
    }
    if (!file.is_open()) {
        std::cerr << "Error: File can't be opened."<< std::endl;;
        return false;
    }
    if (width < 1) {
        std::cerr << "Error: File width size is too small."<< std::endl;
        return false;
    }
    if (height < 1) {
        std::cerr << "Error: File height size is too small."<< std::endl;
        return false;
    }
    return true;
}
void ppmObject::printInfo(){
    std::cout << "--- BMP Header Info ---" << std::endl;
    std::cout << "Width: " << width << " pixels" << std::endl;
    std::cout << "Height: " << height << " pixels" << std::endl;
    std::cout << "Image Size: " << width * height * 3 << " bytes" << std::endl;
    std::cout << "-----------------------" << std::endl;
}
bool ppmObject::isEncryptPossible(const std::string& message)  {
    std::string messageCopy = message;
    const std::vector<bool> messageInBits = textToBits(messageCopy);
    if (!static_cast<size_t>(width) * static_cast<size_t>(height) * 3 >= messageInBits.size()) {
        return false;
    }
    return true;
}
bool ppmObject::encryption(std::string& message){


    std::string messageCopy = message;
    const std::vector<bool> messageInBits = textToBits(messageCopy);
    size_t messageIndex = 0;
    auto formatRgbValue = [](int value) -> std::string {
        if (value < 0 || value > 255) {
            throw std::out_of_range("RGB value out of range");
        }

        std::string result = std::to_string(value);
        while (result.length() < 3) {
            result = "0" + result;
        }

        return result;
    };

    if (magicNumber == "P3") {
        std::fstream file(filePath, std::ios::in | std::ios::out);
        if (!file.is_open()) {
            std::cerr << "Error: File can't be opened.\n";
            return false;
        }

        std::string line;

        int linesSkipped = 0;
        while (linesSkipped < offset && std::getline(file, line)) { ++linesSkipped; }
        file.clear();
        int value;
        while (file >> value) {
            std::streampos readPos = file.tellg();
            if (messageIndex < messageInBits.size()) {
                value = (value & ~1) | static_cast<int>(messageInBits[messageIndex++]);
                std::string fixedValue = formatRgbValue(value);
                file.seekp(readPos - std::streamoff(3));
                file.write(fixedValue.c_str(), 3);
                file.seekg(readPos);
            }
            if (messageIndex >= messageInBits.size()) {
                return true;
            }
        }
    }else if (magicNumber == "P6") {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: File can't be opened.\n";
            return false;
        }


        file.seekg(offset, std::ios::beg);
        file.seekp(offset, std::ios::beg);

        char byte;
        while (file.read(&byte, 1)) {
            if (messageIndex < messageInBits.size()) {
                byte = (byte & ~1) | static_cast<char>(messageInBits[messageIndex++]);

                file.seekp(-1, std::ios::cur);
                file.write(&byte, 1);
                file.seekg(file.tellp());
            } else {
                std::cout << "Message successfully encrypted.\n";
                return true;
            }
        }

        std::cerr << "Error: Message is too long to be hidden in this image.\n";
        return false;
    }


    return true;
}
bool ppmObject::decryption() {
    if (magicNumber == "P3"){
        std::fstream file(filePath, std::ios::in);
        if (!file.is_open()) {
            std::cerr << "Error: File can't be opened.\n";
            return false;
        }
        std::string line;
        int linesSkipped = 0;
        while (linesSkipped < offset && std::getline(file, line)) {
            ++linesSkipped;
        }
        file.clear();
        std::vector<bool> extractedBits;
        int value;
        while (file >> value) {
            bool bit = value & 1;
            extractedBits.push_back(bit);
        }
        std::string message = bitsToText(extractedBits);
        std::cout << "Extracted message: " << message << std::endl;
        return true;
    }else if (magicNumber == "P6") {
        std::fstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: File can't be opened.\n";
            return false;
        }

        file.seekg(offset, std::ios::beg);
        std::vector<bool> extractedBits;
        char byte;

        while (file.read(&byte, 1)) {
            extractedBits.push_back(byte & 1);
        }
        std::string message = bitsToText(extractedBits);
        std::cout << "Extracted message: " << message << std::endl;

        return true;
    }
    return false;
}
