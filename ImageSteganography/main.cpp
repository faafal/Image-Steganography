#include <string>
#include "bmpProcessor.hpp"
#include "ppmProcessor.hpp"
#include <iostream>

void printHelp() {
    std::cout << "Image Steganography - Help\n"
          << "---------------------------------------------\n"
          << "This program allows you to hide and read messages\n"
          << "in image files of type .bmp and .ppm\n\n"
          << "Usage:\n"
          << "  program [flag] [arguments...]\n\n"
          << "Available flags:\n"
          << "  -i, --info [file]          Display image information (size, type, modification date)\n"
          << "  -e, --encrypt [file] [\"message\"]\n"
          << "                             Encrypt (hide) a message in the image\n"
          << "  -d, --decrypt [file]       Read a hidden message from the image\n"
          << "  -c, --check [file] [\"message\"]\n"
          << "                             Check if the message can be stored in the image\n"
          << "  -h, --help                 Display this help screen\n\n"
          << "Notes:\n"
          << "  - The message for -e and -c should be enclosed in quotation marks.\n"
          << "  - Supported formats: .bmp, .ppm\n"
          << "  - Formats like .jpg and .png are not supported without additional libraries\n"
          << "  - In case of syntax errors or missing arguments,\n"
          << "    this help screen will be displayed.\n"
          << "---------------------------------------------\n";

}

std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return filename.substr(dotPos + 1);
}
enum class FileType { UNKNOWN, BMP, PPM };

FileType detectFileType(const std::string& path) {
    std::string ext = getFileExtension(path);
    if (ext == "bmp") return FileType::BMP;
    if (ext == "ppm") return FileType::PPM;
    return FileType::UNKNOWN;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::string flag = argv[1];

    if (flag == "-h" || flag == "--help") {
        printHelp();
        return 0;
    }

    if ((flag == "-i" || flag == "--info") && argc == 3) {
        std::string filePath = argv[2];
        FileType type = detectFileType(filePath);

        switch (type) {
            case FileType::BMP: {
                bmpObject bmp(filePath);
                if (!bmp.isHeaderCorrect()) return std::cerr << "Invalid BMP header\n", 1;
                bmp.printInfo();
                return 0;
            }
            case FileType::PPM: {
                ppmObject ppm(filePath);
                if (!ppm.isHeaderCorrect()) return std::cerr << "Invalid PPM header\n", 1;
                ppm.printInfo();
                return 0;
            }
            default:
                std::cerr << "Unsupported file format.\n";
                return 1;
        }
    }

    if ((flag == "-e" || flag == "--encrypt") && argc == 4) {
        std::string filePath = argv[2];
        std::string message = argv[3];
        FileType type = detectFileType(filePath);

        switch (type) {
            case FileType::BMP: {
                bmpObject bmp(filePath);
                if (bmp.isHeaderCorrect() && bmp.isEncryptPossible(message) && bmp.encryption(message)) {
                    std::cout << "Message encrypted successfully\n";
                    return 0;
                }
                std::cerr << "Error: message encrypted unsuccessfully\n";
                return 1;
            }
            case FileType::PPM: {
                ppmObject ppm(filePath);
                if (ppm.isHeaderCorrect() && ppm.isEncryptPossible(message) && ppm.encryption(message)) {
                    std::cout << "Message encrypted successfully\n";
                    return 0;
                }
                std::cerr << "Error: message encrypted unsuccessfully\n";
                return 1;
            }
            default:
                std::cerr << "Unsupported file format.\n";
                return 1;
        }
    }

    if ((flag == "-d" || flag == "--decrypt") && argc == 3) {
        std::string filePath = argv[2];
        FileType type = detectFileType(filePath);

        switch (type) {
            case FileType::BMP: {
                bmpObject bmp(filePath);
                if (bmp.isHeaderCorrect() && bmp.decryption()) {
                    std::cout << "Message decrypted successfully\n";
                    return 0;
                }
                std::cerr << "Error: message decrypted unsuccessfully\n";
                return 1;
            }
            case FileType::PPM: {
                ppmObject ppm(filePath);
                if (ppm.isHeaderCorrect() && ppm.decryption()) {
                    std::cout << "Message decrypted successfully\n";
                    return 0;
                }
                std::cerr << "Error: message decrypted unsuccessfully\n";
                return 1;
            }
            default:
                std::cerr << "Unsupported file format.\n";
                return 1;
        }
    }

    if ((flag == "-c" || flag == "--check") && argc == 4) {
        std::string filePath = argv[2];
        std::string message = argv[3];
        FileType type = detectFileType(filePath);

        switch (type) {
            case FileType::BMP: {
                bmpObject bmp(filePath);
                if (bmp.isEncryptPossible(message)) {
                    std::cout << "Encrypting following message: \"" + message + "\" is possible\n";
                    return 0;
                }
                std::cerr << "Encrypting following message: \"" + message + "\" is impossible\n";
                return 1;
            }
            case FileType::PPM: {
                ppmObject ppm(filePath);
                if (ppm.isEncryptPossible(message)) {
                    std::cout << "Encrypting following message: \"" + message + "\" is possible\n";
                    return 0;
                }
                std::cerr << "Encrypting following message: \"" + message + "\" is impossible\n";
                return 1;
            }
            default:
                std::cerr << "Unsupported file format.\n";
                return 1;
        }
    }

    std::cerr << "Invalid usage.\n";
    printHelp();
    return 1;
}