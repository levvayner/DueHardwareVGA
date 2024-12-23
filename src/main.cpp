#include "Arduino.h"
#include "DueHardwareVGA.h"
PS2Mouse mouse(PS2_CLK2,PS2_DATA2);
Point mouseLocation;
int zoom = 8;
char key = 0;
enum MousePointer{
    pointerFat = 0,
    pointerSkinny = 1,
    pointerSquare = 2,
    pointerSmall = 3
};

uint8_t *mouseBuffer = nullptr;
uint8_t mouseCursorBuffer[64];
MousePointer pointer = pointerFat;
uint8_t pointers[4][8] = {
    {0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0x1C, 0x0E, 0x06}, //fat
    {0xC0, 0xF0, 0xFC, 0xB0, 0x98, 0x0C, 0x06, 0X03}, //skinny
    {0xF8, 0xE0, 0xF0, 0xB0, 0x98, 0x0C, 0x06, 0X03}, //square
    {0xF0, 0xC0, 0xA0, 0x90, 0x08, 0x04, 0x00, 0x00} //small
};


void processKey(uint8_t data){
    key = data;    
}

void drawCursor(int x, int y, int width, int height){
    
    memcpy(mouseCursorBuffer, mouseBuffer, width*height);
        for(int line=0;line<height; line++){
        uint8_t cursorRowPixels = pointers[(int)pointer][line];
        for(int row=0;row < width; row++){
            if(cursorRowPixels & (0x1 << (7-row)))
                mouseCursorBuffer[(line*width) + row] = 255;
        }
    }
    if(mouseCursorBuffer != nullptr){
            graphics.drawBuffer(mouseLocation.x, mouseLocation.y, width,height, mouseCursorBuffer, btVertical);
        
    }
}

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA Mouse Test");
    graphics.begin();
    graphics.clear();
    for(int vert = 0; vert < graphics.settings.screenHeight / 10; vert++){
        graphics.fillRectangle(0,vert * 10, graphics.settings.screenWidth, graphics.settings.screenHeight / 10, 256/vert);
    }

    keyboard.begin(Serial,50);        
    keyboard.onKeyDown = processKey;

    mouse.begin();    
    Serial.println("Initialized");
}


void loop(){
    if(key != 0 ){
        Serial.print("Received key 0x"); Serial.println(key, HEX);
        graphics.drawText(mouseLocation.x, mouseLocation.y, (char)key, 255);
        key = 0;
    }
    MouseData data = mouse.readData();
    bool mouseChangeEvent = data.status & 0x1 || /*data.wheel != 0 || */ data.position.x != 0 || data.position.y != 0;

    if(data.status & 0x1){
        //left mouse button        
        // graphics.fillRectangle(mouseLocation.x + 1, mouseLocation.y + 1, zoom -2, zoom -2, 250, btVertical);
        // delay(100);
        // graphics.fillRectangle(mouseLocation.x + 1, mouseLocation.y + 1, zoom -2, zoom -2, 0, btVertical);
        pointer = (MousePointer)((int)pointer + 1);
        if(pointer > 3)
        pointer = (MousePointer)0;
        //draw cursor
        drawCursor(mouseLocation.x, mouseLocation.y, zoom, zoom);
        delay(100); //debounce click
    }

    if(mouseChangeEvent){
        //write out buffer contents, if they exist, to current screen place
        //Serial.print("Restoring buffer to screen");
        if(mouseBuffer != nullptr){
                graphics.drawBuffer(mouseLocation.x, mouseLocation.y, zoom,zoom, mouseBuffer, btVertical);
           
        }
        //grab
    }
    // if(data.wheel != 0){
    //     if(data.wheel > 127 )
    //         zoom += 256 - data.wheel;
    //     else
    //         zoom -= data.wheel;

    //     if(zoom <= 1 ) zoom = 2;
    //     if(zoom > 8) zoom = 8;
    //     mouseBuffer = nullptr;
    //     mouseBuffer = new uint8_t[zoom*zoom];
    //     Serial.print("Allocated mouse buffer");
    //     //read in buffer
    //     graphics.readBuffer(mouseLocation, zoom, zoom, mouseBuffer, btVertical);
    //     //draw cursor
    //     drawCursor(mouseLocation.x, mouseLocation.y, zoom, zoom);
    //     //graphics.drawRectangle(mouseLocation.x,mouseLocation.y, zoom - 1, zoom - 1, 127, btVertical);
    // }
    if(data.position.x != 0 || data.position.y != 0){
        //char buf[128];
        
        if(mouseBuffer == nullptr){
            mouseBuffer = new uint8_t[zoom*zoom];
            Serial.print("Allocated mouse buffer");
        }
        
        //update position
        if(data.position.x > 127 ){
            mouseLocation.x -= (256 - data.position.x );
            if(mouseLocation.x < 0) 
                mouseLocation.x = 0;
        }else{
            mouseLocation.x += data.position.x;
            if(mouseLocation.x > graphics.settings.screenWidth) 
                mouseLocation.x = graphics.settings.screenWidth - 2;
        }
        if(data.position.y > 127){
            mouseLocation.y += (256 - data.position.y );
            if(mouseLocation.y > graphics.settings.screenHeight) 
                mouseLocation.y = graphics.settings.screenHeight - 2;        
            
        }else{
            mouseLocation.y -= data.position.y;
            if(mouseLocation.y < 0) 
                mouseLocation.y = 0;
        }

        //read in buffer
        graphics.readBuffer(mouseLocation, zoom, zoom, mouseBuffer, btVertical);

        //draw cursor
        drawCursor(mouseLocation.x, mouseLocation.y, zoom, zoom);
        // sprintf(buf, "New location (%d,%d)",mouseLocation.x, mouseLocation.y   );
        // Serial.println(buf);
    }
    if(mouseChangeEvent) delay(20); //debounce 20ms

}

