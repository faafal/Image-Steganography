#ifndef BMPPROCESSOR_HPP
#define BMPPROCESSOR_HPP

struct bmpObject{
private:
    unsigned int fileSize,dataOffset,headerSize,compression,imageSize,colorsUsed,colorsImportant,colorPlanes,bitsPerPixel;
    unsigned short fileType,bfReserved1,bfReserved2;
    int width,height,xResolution,yResolution,paddingSize;
    std::string filePath;
public:
    bmpObject(const std::string &inputFilePath);
    bool isHeaderCorrect();
    void printInfo();
    bool isEncryptPossible(const std::string& message) ;
    bool encryption(std::string& message) ;
    bool decryption();
};

#endif //BMPPROCESSOR_HPP
