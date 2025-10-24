
#include "sw/Color.h"
struct VRAMSettings{
    uint16_t screenWidth = 800;
    uint16_t screenHeight = 600;
    uint16_t charWidth = 6;
    uint16_t charHeight = 9;
    uint16_t screenBufferHeight = 3000;
    uint16_t horizontalBits = 10;
    uint8_t backgroundColor;
    uint8_t foregroundColor;
    VRAMSettings(uint16_t width, uint16_t height, uint8_t charWidth = 6, uint8_t charHeight = 9, uint8_t foregroundColor = Color::WHITE, uint8_t backgroundColor = Color::DARK_GREEN, uint8_t horizontalBits = 10){
        screenWidth = width;
        screenHeight = height;
        this->charWidth = charWidth;
        this->charHeight = charHeight;
        this->horizontalBits = horizontalBits;
        this->backgroundColor = backgroundColor;
        this->foregroundColor = foregroundColor;
    }
    bool operator ==(const VRAMSettings& other){
       return this->charHeight == other.charHeight &&
            this->charWidth == other.charWidth &&
            this->horizontalBits == other.horizontalBits &&
            this->screenWidth == other.screenWidth &&
            this->screenHeight == other.screenHeight &&
            this->screenBufferHeight == other.screenBufferHeight;
    }
    bool operator !=(const VRAMSettings& other){
        return this->charHeight != other.charHeight ||
            this->charWidth != other.charWidth ||
            this->horizontalBits != other.horizontalBits ||
            this->screenWidth != other.screenWidth ||
            this->screenHeight != other.screenHeight ||
            this->screenBufferHeight != other.screenBufferHeight;
    }
};


