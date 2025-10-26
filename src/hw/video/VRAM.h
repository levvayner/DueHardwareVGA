#ifndef _V_RAM_H
#define _V_RAM_H
#include "SRAM.h"
#include "VRAMSettings.h"
#include "sw/Color.h"
#include "sw/Chars.h"
#include "sw/2D/Graphics2D.h"
#include "sw/3D/Graphics3D.h"



class VRAM : public SRAM{
    
    public:
    VRAMSettings settings = VRAMSettings(800, 600);

    VRAM();
    ~VRAM();

    void begin();
    void end();

    bool isWaiting();
    bool setReady(bool clear = false);
    

    virtual bool drawText(int x, int y, const char * text, byte color = 0xFF, byte backgroundColor = 0x0, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid);
    virtual bool drawText(int x, int y, const char * text, Color color = Color::WHITE, Color backgroundColor = Color::BLACK, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid);

    virtual inline bool drawText(int x, int y, uint8_t * text, byte color = 0xFF, byte backgroundColor = 0x0, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid){
        return drawText(x, y, (const char*)text, color, backgroundColor, clearBackground, useFrameBuffer, busyType);
    }

    virtual inline bool drawText(Point2D location, uint8_t * text, byte color = 0xFF, byte backgroundColor = 0x0, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid){
        return drawText(location.x, location.y, (const char*)text, color, backgroundColor, clearBackground, useFrameBuffer, busyType);
    }
    
    virtual bool drawText(int x, int y, char text, byte color = 0xFF, byte backgroundColor = 0x0, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid);
    virtual bool drawText(int x, int y, char text, Color color = Color::WHITE, Color backgroundColor = Color::BLACK, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid);
    
    virtual bool drawText(Point2D location, char text, byte color = 0xFF, byte backgroundColor = 0x0, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid){
        return drawText(location.x, location.y, text, color, backgroundColor, useFrameBuffer, busyType );
    }
    
    virtual inline bool drawText(int x, int y, unsigned long value, byte color = 0xFF, byte backgroundColor = 0x0, bool clearBackground = true, bool useFrameBuffer = false, BusyType busyType = btInvalid){
        
        memset(_buf,0,sizeof(_buf));
        sprintf(_buf, "%lu", value);
        return drawText(x, y, _buf, color, backgroundColor, clearBackground, useFrameBuffer, busyType);
    }
    

    virtual bool drawTextToBuffer(const char * text, byte* buffer,  uint16_t stride, byte color);
    virtual bool drawTextToBuffer(const char * text, const byte * colors, byte* buffer,  uint16_t stride);

    virtual bool drawBuffer(int x, int y, int width, int height, const byte* buffer, BusyType busyType = btInvalid);

    virtual bool drawPixel(int x, int y, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual bool drawPixel(int x, int y, Color color, BusyType busyType = btInvalid);

    virtual uint8_t readPixel(int x, int y, BusyType busyType = btInvalid);
    virtual bool readBuffer(int x, int y, int width, int height, uint8_t * buffer, BusyType busyType = btInvalid);
    virtual inline bool readBuffer(Point2D location, int width, int height, uint8_t * buffer, BusyType busyType = btInvalid){
        return readBuffer(location.x, location.y, width, height, buffer, busyType);
    }

    virtual bool drawLine(int x1, int y1, int x2, int y2, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual bool drawLine (Point2D start, Point2D end, byte color, BusyType busyType = btInvalid);
    virtual bool drawLine(int x1, int y1, int x2, int y2, Color color = Color::WHITE, BusyType busyType = btInvalid);
    virtual bool drawLine (Point2D start, Point2D end, Color color = Color::WHITE, BusyType busyType = btInvalid);

    virtual bool drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, byte color = 0xFF);
    virtual bool drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
    {
        return drawTriangle(x1, y1, x2, y2, x3, y3, color.ToByte());
    }

    virtual inline bool fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, byte color = 0xFF){
        return _drawTriangle(x1, y1, x2, y2, x3, y3, color, true);
    }
    virtual inline bool fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color = Color::WHITE){
        return fillTriangle(x1, y1, x2, y2, x3, y3, color.ToByte());
    }

    virtual bool drawRectangle(int x1, int y1, int width, int height, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual bool drawRectangle(Point2D topLeft, Point2D bottomRight, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual bool drawRectangle(int x1, int y1, int width, int height, Color color = Color::WHITE, BusyType busyType = btInvalid);
    virtual bool drawRectangle(Point2D topLeft, Point2D bottomRight, Color color = Color::WHITE, BusyType busyType = btInvalid);

    virtual bool fillRectangle(int x1, int y1, int width, int height, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual bool fillRectangle(int x1, int y1, int width, int height,  Color color =Color::WHITE, BusyType busyType = btInvalid);
    virtual bool fillRectangle(Point2D topLeft, Point2D bottomRight, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual bool fillRectangle(Point2D topLeft, Point2D bottomRight, Color color =Color::WHITE, BusyType busyType = btInvalid);

    virtual bool drawCircle(int centerX, int centerY, int radius, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual inline bool drawCircle(int centerX, int centerY, int radius, Color color = Color::WHITE, BusyType busyType = btInvalid){
        return drawCircle(centerX, centerY, radius, color.ToByte(), busyType);
    }

    virtual bool drawArc(int x, int y, int startAngle, int endAngle, int radius, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual inline bool drawArc(int x, int y, int startAngle, int endAngle, int radius, Color color = Color::WHITE, BusyType busyType = btInvalid){
        return drawArc(x, y, startAngle, endAngle, radius, color.ToByte(), busyType);
    }

    virtual bool fillCircle(int x, int y, int radius, byte color = 0xFF, BusyType busyType = btInvalid);
    virtual inline bool fillCircle(int x, int y, int radius, Color color = Color::WHITE, BusyType busyType = btInvalid) { 
        return fillCircle(x, y, radius, color.ToByte(), busyType);
    }

    virtual bool drawOval(int centerX, int centerY, int width, int height, byte color = Color::WHITE, BusyType busyType = btInvalid);
    inline virtual bool drawOval(int centerX, int centerY, int width, int height, Color color = Color::WHITE, BusyType busyType = btInvalid){
        return drawOval(centerX, centerY, width, height, color.ToByte(), busyType);
    }

    virtual bool fillOval(int centerX, int centerY, int width, int height, byte color = Color::WHITE, BusyType busyType = btInvalid);
    

    virtual inline bool clear(int x1 = 0, int y1 = 0, int width = 0, int height = 0, BusyType busyType = btInvalid){
        if(width == 0) width = settings.screenWidth - x1 + 2;
        if(height == 0) height = settings.screenHeight - y1 + 2;
        Serial.print("Clearing area at "); Serial.print(x1); Serial.print(", "); Serial.print(y1); Serial.print(" width: "); Serial.print(width); Serial.print(" height: "); Serial.print(height); Serial.print(" with color: "); Serial.println(settings.backgroundColor);
        return fillRectangle(x1, y1, width, height, settings.backgroundColor, busyType);
    }

    /// @brief renders frame buffer to screen (writes to ram)
    virtual bool render();


    virtual inline void SetRenderMode(BusyType waitType){ _waitType = waitType;}


    static inline  uint8_t mulitplyColors(uint8_t a, uint8_t b) {
        uint8_t s = a + b;
        uint8_t m = (s - ((a ^ b) & 0x01010101)) & 0x01010101;
	    return m * 0xFF >> 8 | (s - m);
    }
    static inline uint8_t averageColors(uint8_t a, uint8_t b){
        return (((a>> 5) + (b >> 5)) >> 1) << 5 | 
        ((((a >> 3 )& 0x7) + ((b >> 3)  & 0x7)) >> 1) << 3 | 
        ((((a & 0x3) + (b & 0x3)) >> 1) & 0x3);
    }


    static inline  uint8_t diffColors(uint8_t a, uint8_t b) {
        return ((a>> 5) + (b >> 5) / 2) << 5 | 
        (((a >> 3 )& 0x7) + ((b >> 3)  & 0x7)/ 2) << 3 | 
        ((((a & 0x3) + (b&0x3))/2) & 0x3);
        // uint8_t s = a + b;
        // uint8_t m = (s - ((a ^ b) & 0x01010101)) & 0x01010101;
	    // return s - m;
    }

    private:
    bool _drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, byte color = 0xFF, bool fill = false);
    void _drawTriangleTop2(int x1, int x2, int topY, int x3, int bottomY, byte color, bool fill = false);
    void _drawTriangleTop1EqualBottoms(int topX, int topY, int x1, int x2, int bottomY, byte color, bool fill = false);
    void _drawTriangleTop1DifferentBottoms(int topX, int topY, int middleX, int middleY, int bottomX, int bottomY, byte color, bool fill = false);

    void _drawTrinagleScanLines(TrinagleLegDrawObject & leg1, TrinagleLegDrawObject & leg2, int topY, int bottomY, byte color, bool fill);
    private:
    uint8_t *_frameBuffer;
    char _buf[16];

    BusyType _waitType;

};
#endif

extern VRAM graphics;
