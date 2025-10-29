#ifndef textColor_H_
#define textColor_H_
#include "stdint.h"

class Color {

    public:
    Color() : _red(0x7), _green(0x7), _blue(0x3) {}
    Color(uint8_t red, uint8_t green, uint8_t blue);
    Color(uint8_t color);
    Color(Color *source);

    
        
    inline uint8_t Red(){ return _red;}
    inline uint8_t Green(){ return _green;}
    inline uint8_t Blue(){ return _blue;}

    inline void FromByte(uint8_t color){
        _red = (color >> 5) & 0x7;
        _green = (color >> 2) & 0x7;
        _blue = color & 0x3;
    }

    inline uint8_t ToByte(){ return (_red << 5) | ((_green & 0x7) << 2) | (_blue & 0x3);}

       
    static inline Color FromRGB(uint8_t red, uint8_t green, uint8_t blue){
        return Color(red,green,blue);
    }
    const static uint8_t WHITE = 0xFF;
    const static uint8_t GRAY = 111;
    const static uint8_t BLACK = 0x0;
    const static uint8_t GREEN = 150;
    const static uint8_t BLUE = 0x3;
    const static uint8_t YELLOW  = 244;
    const static uint8_t GOLD = 172;
    const static uint8_t ORANGE = 112;    
    const static uint8_t PEACH = 242;
    const static uint8_t RED = 192;
    const static uint8_t TEAL = 159;
    const static uint8_t PURPLE = 66 ;
    const static uint8_t LIME = 212;
    const static uint8_t BRICK = 176;
    const static uint8_t MAROON = 120;
    const static uint8_t BROWN = 208;
    const static uint8_t NAVY_BLUE = 1;
    const static uint8_t LIGHT_BLUE = 155;
    const static uint8_t LIGHT_GREEN = 220;
    const static uint8_t DARK_GREEN = 10;

    private:
        uint8_t _red;
        uint8_t _green;
        uint8_t _blue;
};
#endif
