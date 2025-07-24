#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "bmpProcessor.hpp"
#include "helpFunctions.hpp"

bmpObject::bmpObject(const std::string& inputFilePath) {
    filePath = inputFilePath;
    fileSize = 0,dataOffset = 0,headerSize = 0,compression = 0,imageSize = 0,colorsUsed = 0,colorsImportant = 0,colorPlanes = 0,bitsPerPixel = 0;
    width = 0,height = 0,xResolution = 0,yResolution = 0;paddingSize = 0,bfReserved1 = 0,bfReserved2 = 0,fileType = 0;
}
bool bmpObject::isHeaderCorrect() {
    std::fstream file(filePath,std::ios::in |std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: File can't be opened."<< std::endl;;
        return false;
    }
    fileType = static_cast<unsigned short>(readLittleEndian(file, 2, EndianReadType::USHORT));
    if (fileType != 0x4D42) {
        std::cerr << "Error: Not a valid BMP file (invalid signature)." << std::endl;
        return false;
    }

    fileSize = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));
    bfReserved1 = static_cast<unsigned short>(readLittleEndian(file, 2, EndianReadType::USHORT));
    bfReserved2 = static_cast<unsigned short>(readLittleEndian(file, 2, EndianReadType::USHORT));
    dataOffset = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));
    headerSize = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));

    // Standard size BITMAPINFOHEADER = 40 bytes
    if (headerSize >= 40) {
        width = static_cast<int>(readLittleEndian(file, 4, EndianReadType::INT));
        height = static_cast<int>(readLittleEndian(file, 4, EndianReadType::INT));
        colorPlanes = static_cast<unsigned short>(readLittleEndian(file, 2, EndianReadType::USHORT));
        bitsPerPixel = static_cast<unsigned short>(readLittleEndian(file, 2, EndianReadType::USHORT));
        compression = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));
        imageSize = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));
        xResolution = static_cast<int>(readLittleEndian(file, 4, EndianReadType::INT));
        yResolution = static_cast<int>(readLittleEndian(file, 4, EndianReadType::INT));
        colorsUsed = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));
        colorsImportant = static_cast<unsigned int>(readLittleEndian(file, 4, EndianReadType::UINT));

        if (width > 0 && bitsPerPixel > 0) {
            int bytes_per_pixel_calc = bitsPerPixel / 8;
            if (bytes_per_pixel_calc == 0 && bitsPerPixel > 0) bytes_per_pixel_calc = 1;
            if (bytes_per_pixel_calc > 0) {
                paddingSize = (4 - ((width * bytes_per_pixel_calc) % 4)) % 4;
            } else {
                paddingSize = 0;
            }
        } else {
            paddingSize = 0;
        }

        if (compression != 0) {
            std::cerr << "Error: Compressed BMP formats are not supported (compression method: " << compression << ")."<< std::endl;;
            return false;
        }
        if(bitsPerPixel != 24) {
            std::cerr << "Error: Only 24-bit BMP format is supported (bits per pixel: " << bitsPerPixel << ")."<< std::endl;;
            return false;
        }
    }else {
        std::cerr << "Unsupported BMP info header size (" << headerSize << " bytes). Expected at least 40 bytes."<< std::endl;;
        return false;
    }
    return true;
}
void bmpObject::printInfo(){
    std::cout << "--- BMP Header Info ---" << std::endl;
    std::cout << "File Size: " << fileSize << " bytes" << std::endl;
    std::cout << "Data Offset: " << dataOffset << " bytes" << std::endl;
    std::cout << "Header Size: " << headerSize << " bytes" << std::endl;
    std::cout << "Width: " << width << " pixels" << std::endl;
    std::cout << "Height: " << height << " pixels" << std::endl;
    std::cout << "Bits Per Pixel: " << bitsPerPixel << std::endl;
    std::cout << "Compression: " << (compression == 0 ? "None" : std::to_string(compression)) << std::endl;
    std::cout << "Image Size: " << ((width*bitsPerPixel+31)/32)*4*height << " bytes" << std::endl;
    std::cout << "-----------------------" << std::endl;
}
bool bmpObject::isEncryptPossible(const std::string& message)  {
    std::string messageCopy = message;
    const std::vector<bool> messageInBits = textToBits(messageCopy);
    if (!static_cast<size_t>(width)*static_cast<size_t>(std::abs(height))*3 >= messageInBits.size()) {
        return false;
    }
    return true;
}
bool bmpObject::encryption(std::string& message){
    size_t messageIndex = 0;
    std::string messageCopy = message;
    const std::vector<bool>& messageInBits = textToBits(messageCopy);
    auto encodeSingleLSB = [&](unsigned char& channel_byte) -> bool {
        if (messageIndex < messageInBits.size()) {
            const bool message_bit = messageInBits[messageIndex];
            channel_byte = (channel_byte & ~1) | static_cast<unsigned char>(message_bit);
            messageIndex++;
            return true;
        }
        return false;
    };
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: File can't be opened."<< std::endl;;
        return false;
    }

    file.seekg(dataOffset, std::ios::beg);
    bool message_embedded_completely = false;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char b, g, r;
            const long long current_pixel_pos = file.tellg();
            if (!file.read(reinterpret_cast<char*>(&b), 1) ||
                !file.read(reinterpret_cast<char*>(&g), 1) ||
                !file.read(reinterpret_cast<char*>(&r), 1)) {
                std::cerr << "Error reading pixel data at row " << y << ", pixel " << x << std::endl;
                return false;
            }
            if (!message_embedded_completely) {
                if (!encodeSingleLSB(b)) {message_embedded_completely = true; }
            }
            if (!message_embedded_completely) {
                if (!encodeSingleLSB(g)) {message_embedded_completely = true; }
            }
            if (!message_embedded_completely) {
                if (!encodeSingleLSB(r)) {message_embedded_completely = true; }
            }
            file.seekp(current_pixel_pos, std::ios::beg);
            if (!file.write(reinterpret_cast<char*>(&b), 1) ||
                !file.write(reinterpret_cast<char*>(&g), 1) ||
                !file.write(reinterpret_cast<char*>(&r), 1)) {
                std::cerr << "Error writing pixel data at row " << y << ", pixel " << x << std::endl;
                return false;
            }
        }
        if (message_embedded_completely) {
            break;
        }
        file.seekg(paddingSize, std::ios::cur);
        file.seekp(paddingSize, std::ios::cur);
    }
    return true;
}
bool bmpObject::decryption() {
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: File can't be opened."<< std::endl;;
        return false;
    }
    std::vector<char> pixelsChannels;
    pixelsChannels.reserve(static_cast<size_t>(width) * static_cast<size_t>(std::abs(height)) * 3);
    file.seekg(dataOffset, std::ios::beg);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char b, g, r;
            // Read BGR bytes
            if (!file.read(reinterpret_cast<char*>(&b), 1) ||
                !file.read(reinterpret_cast<char*>(&g), 1) ||
                !file.read(reinterpret_cast<char*>(&r), 1)) {
                std::cerr << "Error reading pixel data at row " << y << ", pixel " << x << std::endl;
                pixelsChannels.clear();
                return false;
            }
            pixelsChannels.emplace_back(b);
            pixelsChannels.emplace_back(g);
            pixelsChannels.emplace_back(r);
        }
        // Skip padding bytes at the end of the row
        file.seekg(paddingSize, std::ios::cur);
    }

    auto extractBit = [&](unsigned char byte) -> bool {
        return (byte & 1);
    };
    std::vector<bool> extractedBits;
    extractedBits.reserve(static_cast<size_t>(width) * static_cast<size_t>(height) * 3);
    for (const auto& channel : pixelsChannels) {
        extractedBits.push_back(extractBit(channel));
    }
    std::cout << "Extracted message: " << bitsToText(extractedBits) << std::endl;
    return true;
}
