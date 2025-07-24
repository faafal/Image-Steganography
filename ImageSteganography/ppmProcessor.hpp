#ifndef PPMPROCESSOR_HPP
#define PPMPROCESSOR_HPP
#include <vector>

struct ppmObject{
private:
    std::string filePath;
    std::string magicNumber;
    std::vector<std::string> comments;
    int width;
    int height;
    int maxChannelValue;
    int offset;
public:
    ppmObject(const std::string &inputFilePath);
    bool isHeaderCorrect();
    void printInfo();
    bool isEncryptPossible(const std::string& message);
    bool encryption(std::string& message);
    bool decryption();
};

#endif //PPMPROCESSOR_HPP
