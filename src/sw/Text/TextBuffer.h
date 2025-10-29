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

    void SetIsPrinted1(uint8_t* flag, bool isPrinted1 ){
        if(isPrinted1)
            *flag &= 1 << 0x0;
        else
            *flag = *flag & ~(1 << 0);
    }
    void SetIsPrinted2(uint8_t* flag, bool isPrinted2 ){
        if(isPrinted2)
            *flag &= 1 << 0x1;
        else
            *flag = *flag & ~(1 << 1);
    }
    void SetUnderlined(uint8_t* flag, bool isUnderlined){
        if(isUnderlined)
            *flag &= 1 << 0x2;
        else
            *flag = *flag & ~(1 << 2);
        
    }
    void SetIsTransparentBackground(uint8_t* flag, bool isTransparent){
        if(isTransparent)
            *flag &= 1 << 0x3;
        else
            *flag = *flag & ~(1 << 3);
        
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
        //Serial.print("Initialized text buffer with"); Serial.print(width*height); Serial.print(" chars");
        Clear();
    }

    void Add(uint16_t x, uint16_t y, char character, uint8_t color, uint8_t bgcolor, bool isTransparent = false){
        if(x >= width || y >= height || x < 0 || y < 0) return;
        //Serial.print("Adding char"); Serial.print(character); Serial.print(" at ("); Serial.print(x); Serial.print(","); Serial.print(y); Serial.print(") with color "); Serial.println(color.ToByte());
        text[y * width + x] = character;
        colors[y * width + x] = color;
        bgcolors[y * width + x] = bgcolor;
        SetIsTransparentBackground( &flags[y * width + x], isTransparent);
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
    void UpdateChar(uint16_t x, uint16_t y, char character){
        if(x >= width || y >= height) return;
        text[y * width + x] = character;
        InvalidateChar(x,y);
    }

    void UpdateCharUnderline(uint16_t x, uint16_t y, bool underline){
        if(x >= width || y >= height) return;
        SetUnderlined( &flags[y * width + x], underline);
        
        InvalidateChar(x,y);
    }   
    void InvalidateChar(uint16_t x, uint16_t y){
        if(x >= width || y >= height) return;
        SetIsPrinted1( &flags[y * width + x], false);
        SetIsPrinted2( &flags[y * width + x], false);        
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

    void Clear(){
        // delete text;
        // delete colors;
        // delete flags;
        if(text != nullptr)
            memset(text, 0, width * height * sizeof(char));
        if(text != nullptr)
            memset(colors, 0, width * height * sizeof(uint8_t));
        if(text != nullptr)
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