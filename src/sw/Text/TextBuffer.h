#ifndef TEXTBUFFER_H
#define TEXTBUFFER_H

#include "sw/Color.h"
#include "sw/GPU.h"
#include <cstring>

// struct TextChar{
//     char character;
//     uint8_t color;
//     bool isPrintedBuffer1 = false;
//     bool isPrintedBuffer2 = false;
//     TextChar(): character(' '), color(Color::WHITE){};
//     TextChar(char character, Color color): character(character), color(color.ToByte()){};
// };
// struct TextFlags{
//     bool isPrintedBuffer1 = false;
//     bool isPrintedBuffer2 = false;
//     bool isUnderlined = false;
// };

class TextBuffer{
    public:
    uint16_t width;
    uint16_t height;
    char* text;
    uint8_t* colors;
    uint8_t* bgcolors;
    uint8_t* flags;

   
    bool GetIsPrinted1(uint8_t* flag){
        return *flag & 0x1 << 0;
    }
    bool GetIsPrinted2(uint8_t* flag){
        return *flag & 0x1 << 1;
    }
    bool GetUnderlined(uint8_t* flag){
        return *flag & 0x1 << 2;
    }
    bool GetTransparentBackground(uint8_t* flag){
        return *flag & 0x1 << 3;
    }
    void SetIsPrinted1(uint8_t idx, bool isPrinted1 ){
        //Serial.print("Setting is printed1 flag to "); Serial.println(isPrinted1 ? "printed" : "not printed");
        if(isPrinted1)
            flags[idx] |= 0x1;
        else
            flags[idx] &= ~(1 << 0);
    }
    void SetIsPrinted2(uint8_t idx, bool isPrinted2 ){
        //Serial.print("Setting is printed2 flag to "); Serial.println(isPrinted2 ? "printed" : "not printed");
        if(isPrinted2)
            flags[idx] |= 1 << 0x1;
        else
            flags[idx] &= ~(1 << 1);
    }
    void SetUnderlined(uint8_t idx, bool isUnderlined){
        //Serial.print("Setting underlined flag to "); Serial.println(isUnderlined ? "underlined" : "not underlined");
        if(isUnderlined)
            flags[idx] |= 1 << 0x2;
        else
            flags[idx] &= ~(1 << 2);
    }
    void SetIsTransparentBackground(uint8_t idx, bool isTransparent){
        if(isTransparent)
            flags[idx] |= 1 << 0x3;
        else
            flags[idx] &= ~(1 << 3);
        
    }


    //TextChar* buffer;
    TextBuffer(){
        width = 0;//graphics.settings.screenWidth / graphics.settings.charWidth;
        height = 0;//graphics.settings.screenHeight / graphics.settings.charHeight;
        text = new char[width * height];
        colors = new uint8_t[width * height];
        bgcolors = new uint8_t[width * height];
        flags = new uint8_t[width * height];
        //Serial.print("Initialized text buffer with"); Serial.print(width*height); Serial.print(" chars");
    
        Clear();
    }

    TextBuffer(uint16_t width, uint16_t height){
        this->width = width;
        this->height = height;
        text = new char[width * height]{0};
        colors = new uint8_t[width * height] {0};
        bgcolors = new uint8_t[width * height] {0};
        flags = new uint8_t[width * height] {0};
        Serial.print("Initialized text buffer with "); Serial.print(width*height); Serial.println(" chars");
        Clear();
    }

    void Add(uint16_t x, uint16_t y, char character, uint8_t color, uint8_t bgcolor, bool isTransparent = false){
        if(x >= width || y >= height || x < 0 || y < 0) return;
        //Serial.print("Adding char "); Serial.print(character); Serial.print(" at ("); Serial.print(x); Serial.print(","); Serial.print(y); Serial.print(") with color "); Serial.println(color);
        text[y * width + x] = character;
        colors[y * width + x] = color;
        bgcolors[y * width + x] = bgcolor;
        //flags[y * width + x] |= isTransparent << 3;
        SetIsTransparentBackground( y * width + x, isTransparent);
        InvalidateChar(x,y);
    }

    // void Add(uint16_t x, uint16_t y, TextChar textChar){
    //     if(x >= width || y >= height) return;
    //     text[y * width + x] = textChar.character;
    //     colors[y * width + x] = textChar.color;
    //     InvalidateChar(x,y);
    // }
    void UpdateCharColor(uint16_t x, uint16_t y, uint8_t color, uint8_t bgcolor){
        if(x >= width || y >= height) return;
        colors[y * width + x] = color;
        bgcolors[y * width + x] = bgcolor;
        InvalidateChar(x,y);
    }
    void UpdateChar(Point2D point, char character){
        UpdateChar(point.x,point.y, character);
    }
    void UpdateChar(uint16_t x, uint16_t y, char character){
        if(x >= width || y >= height) return;
        Serial.print("GPU Text buffer: Updating character "); Serial.print(text[y * width + x]); Serial.print("to ["); Serial.print(character); Serial.println("]");
        text[y * width + x] = character;
        InvalidateChar(x,y);
    }

    void ClearChar(uint16_t x, uint16_t y){
        if(x >= width || y >= height) return;
        //Serial.print("GPU Text buffer: Clearing character "); Serial.println(text[y * width + x]);
        text[y * width + x] = ' ';
        //SetIsTransparentBackground(y * width + x,false); //paint over console
        flags[y * width + x] &= 0xF0;
    }
    /// @brief Moves text back one character from specified position to first null terminator
    /// @param x 
    /// @param y 
    void RemoveChar(uint16_t x, uint16_t y){
        if(x >= width || y >= height) return;
        //move remaining text back one
        char buf[128];
        int bytesToCopy = 128;
        //scan for end
        int bytesLeft = 0;
        
        for(int idx = y * width + x; idx < width * height; idx++){
            if(text[idx] == 0) break;
            bytesLeft++;
            flags[idx] &= ~(0x3); //mark char to be redrawn
        }
        int lastIdx = y * width + x + bytesLeft - 1;

        for(int idx = y * width + x; idx < bytesLeft + idx; idx += bytesToCopy)
        {
            bytesToCopy = bytesToCopy < bytesLeft ? bytesToCopy : bytesLeft;
            if(bytesToCopy == 0) break;
            Serial.print("Copying "); Serial.print(bytesToCopy); Serial.println(" bytes.");
            //text
            memcpy(buf,text + idx,bytesToCopy);
            memcpy(text + idx - 1, buf,bytesToCopy);
            //color
            memcpy(buf,colors + idx,bytesToCopy);
            memcpy(colors + idx - 1, buf,bytesToCopy);
            //bgcolor
            memcpy(buf,bgcolors + idx,bytesToCopy);
            memcpy(bgcolors + idx - 1, buf,bytesToCopy);
            //flags
            memcpy(buf,flags + idx,bytesToCopy);
            memcpy(flags + idx - 1, buf,bytesToCopy);

            bytesLeft -= bytesToCopy;

        }
        
        //clear last char
        text[lastIdx] = 0;
        flags[lastIdx] = 0;
    }

    void UpdateCharUnderline(uint16_t x, uint16_t y, bool underline){
        
        if(x >= width || y >= height) return;
        int idx = y * width + x;
        //Serial.print("Update "); Serial.print(text[idx]); Serial.print(" -- set underline at ("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") to "); Serial.println(underline ? "ON" : "OFF");
        //Serial.print("Start with flag: b'"); Serial.print(flags[idx] ,BIN);
        //SetUnderlined(idx,underline);
        //InvalidateChar(x,y);
        flags[idx] &= 0xF8;
        if(underline){
            flags[idx] |= 1 << 2;
            if(text[idx] == 0)
                text[idx] = 1;
        }
            // if(text[idx] == 1)
            //     text[idx] = 0;
        //text[idx] = '-';
        //colors[idx] = 255;
        //Serial.print(" Set Char flags to: b'"); Serial.println(flags[idx] ,BIN);
    }   
    void InvalidateChar(uint16_t x, uint16_t y){
        if(x >= width || y >= height) return;        
        flags[y * width + x] &= ~(0x3);      
    }
    
    void AddString(uint16_t x, uint16_t y, const char* str, uint8_t color, uint8_t bgColor, bool isTransparent = false){
        uint16_t posX = x;
        uint16_t posY = y;
        for(size_t i = 0; i < strlen(str); i++){
            if(posX >= width){
                posX = 0;
                posY++;
                if(posY >= height) break;
            }
            Add(posX, posY, str[i], color, bgColor, isTransparent);
            posX++;
        }
    }

    size_t GetLineLength(uint16_t lineNumber){
        auto data = new char[width] {0};
        memcpy(data,text + lineNumber * width,width);
        size_t ret = strlen(data);
        delete[] data;
        return ret;
    }

    void ScrollDown(){
        auto data = new char[width] {0};
        for(int line = 0; line < height - 1; line++){
            //replace contents with next line            
            memcpy(data,text + (line+1)*width,width);
            memcpy(text + line*width,data,width);
            //Serial.print("Copied line ["); Serial.print(data); Serial.print("] from line "); Serial.print(line + 1); Serial.print(" to line "); Serial.println(line);

            memcpy(data,colors + (line+1)*width,width);
            memcpy(colors + line*width,data,width);

            memcpy(data,bgcolors + (line+1)*width,width);
            memcpy(bgcolors + line*width,data,width);

            memcpy(data,flags + (line+1)*width,width);
            memcpy(flags + line*width,data,width);
            
        }
        memset(text+(height-1)*width, 0, width);
        memset(colors+(height-1)*width, 0, width);
        memset(bgcolors+(height-1)*width, 0, width);
        memset(flags+(height-1)*width, 0, width);
        Invalidate();
        delete[] data;
    }

    void Invalidate(){
        for(int idx=0;idx < width * height;idx++){
            flags[idx] &= 0xFC; // clear lowest two bits
        }        
    }

    void Clear(){
        if(text != nullptr)
            memset(text, 0, width * height * sizeof(char));
        if(colors != nullptr)
            memset(colors, 0, width * height * sizeof(uint8_t));
        if(bgcolors != nullptr)
            memset(bgcolors, 0, width * height * sizeof(uint8_t));
        if(flags != nullptr)
            memset(flags, 0, width * height * sizeof(uint8_t));
    }

    ~TextBuffer(){
        delete[] text;
        text = nullptr;
        delete[] colors;
        colors = nullptr;
        delete[] flags;
        flags = nullptr;
    }
};
#endif // TEXTBUFFER_H