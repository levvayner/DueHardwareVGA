#ifndef GPU_CPP
#define GPU_CPP
#include "2D/Enums2D.h"
#include "GPU.h"
//#define DEBUG_GPU

extern VRAM graphics;
GPU gpu(RenderMode::rmBuffered);
extern volatile bool __isBufferReadySet;
volatile bool __activeBank;

extern char _end;
extern "C" char* sbrk(int i);

void clearReadySet(){
    __isBufferReadySet = false;
    __activeBank = !__activeBank;
    digitalWrite(PIN_READY,LOW); 
}

void GPU::PrintRam(Print &print)
{
   
    char* heapend = sbrk(0);
    register char* stack_ptr asm("sp");
    struct mallinfo mi = mallinfo();

    if (MinfreeRAM > stack_ptr - heapend + mi.fordblks || MinfreeRAM == 0)
    { 
        MinfreeRAM = stack_ptr - heapend + mi.fordblks;
        //automatically reboot if we are below threshold
        if(MinfreeRAM < CRITICAL_MEMORY_RESTART_THRESHOLD)
            RSTC->RSTC_CR = 0xA5000005; // Reset processor and internal peripherals
            //TODO: store executing command, load after restart
    }
    print.print("Min FREE RAM: "); print.println(MinfreeRAM);

}

GPU::GPU(RenderMode mode = RenderMode::rmDirect)
{
    _renderMode = mode;   
}

void GPU::begin()
{
    _renderCanvas.shapeList = new ShapeList<GraphicsObject2D>();
    _mouseCanvas.shapeList = new ShapeList<GraphicsObject2D>();
    Serial.println("** Creating text buffer. Memory Before:  ");
    PrintRam(Serial);
    _textBuffer = *new TextBuffer(graphics.settings.screenWidth/graphics.settings.charWidth ,graphics.settings.screenHeight/graphics.settings.charHeight);
    Serial.print("Memory after:  "); PrintRam(Serial);
    
    graphics.begin(graphics.settings.screenWidth, graphics.settings.screenHeight, graphics.settings.foregroundColor);
    
    //graphics.settings.backgroundColor = 0x00;
    attachInterrupt(digitalPinToInterrupt(PIN_BANK_SELECT), clearReadySet, CHANGE);
}

void GPU::end()
{
    ClearScreen();    
    detachInterrupt(digitalPinToInterrupt(PIN_BANK_SELECT));
}

void GPU::Render()
{
    bool modified = false;
    unsigned long startTime = millis();
    while(graphics.isWaiting() && (millis() - startTime < 20));
    //_activeBank = digitalRead(PIN_BANK_SELECT);
    #ifdef DEBUG_GPU
    Serial.print("Rendering to bank "); Serial.println(__activeBank);
    Serial.print("Objects to render: "); Serial.println(_renderCanvas.shapeList->size());
    #endif
    if(__activeBank == 0 && !_isBank1Initialized){
        
        graphics.clear(0,0,graphics.settings.screenWidth, graphics.settings.screenHeight); //clear full screen
        _isBank1Initialized = true;
        #ifdef DEBUG_GPU
        Serial.println("Initialized bank 0");
        #endif
    }
    else if(__activeBank == 1 && !_isBank2Initialized){
        graphics.clear(0,0, graphics.settings.screenWidth, graphics.settings.screenHeight);
        _isBank2Initialized = true;
        #ifdef DEBUG_GPU
        Serial.println("Initialized bank 1");
        #endif
    }

    if(_renderCanvas.shapeList != nullptr && _renderCanvas.shapeList->size() > 0){
        #ifdef DEBUG_GPU
        Serial.print("Rendering ");Serial.print(_renderCanvas.shapeList->size() ); Serial.println(" graphics objects");
        Serial.print("Empty flag: "); Serial.println(_renderCanvas.shapeList->empty() ? "Empty" : "Contains Data");
        #endif
        for(auto &obj : *_renderCanvas.shapeList){
            if(__activeBank == 0 && !obj.drawnOnMem1){
                Draw2DObject(&obj);
                obj.drawnOnMem1 = true;
                modified = true;
            }
            else if(__activeBank == 1 && !obj.drawnOnMem2){
                Draw2DObject(&obj);
                obj.drawnOnMem2 = true;
                modified = true;
            } 
        }
    }

    if(_renderMode == RenderMode::rmText){
        // in text mode, just clear the screen
        //graphics.clear();
        modified |= DrawTextBuffer();
    }
    // else if(_renderMode == RenderMode::rmDirect){
    //     // in direct mode, draw all objects every frame
    //     for(auto &obj : *_renderCanvas.shapeList){
    //         Draw2DObject(&obj);
    //     }
    // }
    //else if(_renderMode == RenderMode::rmBuffered){
        // in buffered mode, only draw objects that haven't been drawn to the active bank yet
        
   //}

    //mouse canvas    
    // if(_mouseCanvas.shapeList != nullptr && _mouseCanvas.shapeList->size() > 0){
    //     #ifdef DEBUG_GPU
    //     Serial.print("Rendering mouse ");Serial.print(_mouseCanvas.shapeList->size() ); Serial.println(" graphics objects");
    //     Serial.print("Empty flag: "); Serial.println(_mouseCanvas.shapeList->empty() ? "Empty" : "Contains Data");
    //     #endif
    //     for(auto &obj : *_mouseCanvas.shapeList){
    //         if(__activeBank == 0 && !obj.drawnOnMem1){
    //             // Serial.print("DRAWING MOUSE  ON BANK 1"); 
    //             DrawMouseObject(&obj);
    //             //Draw2DObject(&obj);
    //             obj.drawnOnMem1 = true;
    //             modified = true;
    //         }
    //         else if(__activeBank == 1 && !obj.drawnOnMem2){
    //             // Serial.print("DRAWING MOUSE  ON BANK 2"); 
    //             DrawMouseObject(&obj);
    //             //Draw2DObject(&obj);
    //             obj.drawnOnMem2 = true;
    //             modified = true;
    //         }
    //     }
    // }
    
    if(modified)
        graphics.setReady();
    //Serial.print("Rendering frame: "); Serial.print(millis() - startTime); Serial.println(" ms");
}

void GPU::Invalidate()
{
    _isBank1Initialized = false;
    _isBank2Initialized = false;
    for(auto &obj : *_renderCanvas.shapeList){
        obj.drawnOnMem1 = false;
        obj.drawnOnMem2 = false;
    }
    _textBuffer.Invalidate();
}

bool GPU::activeBank()
{
    return __activeBank;
}

void GPU::Add2DObject(const GraphicsObject2D &obj)
{
    _renderCanvas.shapeList->push_back(std::move(obj));
}

bool GPU::DrawTextBuffer()
{
    bool modified = false;
    if(_textBuffer.text == nullptr){
        Serial.print("No text to draw ");
        return false;
    }
    //Serial.print("Drawing from text buffer "); Serial.print(_textBuffer.width); Serial.print("x"); Serial.println(_textBuffer.height);
    for(int line = 0; line < _textBuffer.height; line++){
        for(int col = 0; col < _textBuffer.width; col++){
            int idx = (line * _textBuffer.width) + col;
            char character = _textBuffer.text[idx];
            uint8_t color = _textBuffer.colors[idx];
            uint8_t bgColor = _textBuffer.bgcolors[idx];
            uint8_t flag = _textBuffer.flags[idx];
           
            auto isPrinted1 = flag & 1 << 0;
            auto isPrinted2 = flag & 1 << 1;
            auto isUnderlined = flag & 1 << 2;
            auto isTransparent = flag & 1 << 3;
          
            //skip printing if already printed
            if((isPrinted1 && __activeBank == 0) || (isPrinted2 && __activeBank == 1))
            {              
                continue;
            }

            if(__activeBank == 0)
                _textBuffer.flags[idx] |= 0x1;
                //_textBuffer.SetIsPrinted1(idx,true);
            else
                _textBuffer.flags[idx] |= 0x2;  

            // if(isUnderlined){
            //     Serial.print(" char at ["); Serial.print(col); Serial.print(", "); Serial.print(line); Serial.print("] is underlined!");
            // }
            if( character == 0)
                continue;       
            modified = true;
            graphics.drawLine(col * graphics.settings.charWidth, (line + 1) * graphics.settings.charHeight ,(col + 1) * graphics.settings.charWidth, (line + 1) * graphics.settings.charHeight, isUnderlined ? graphics.settings.foregroundColor : bgColor);
            
            // Serial.print("["); Serial.print(millis()); Serial.print("] "); Serial.print("Drawing character "); Serial.print(character); Serial.print(" at ["); Serial.print(col); Serial.print(", "); Serial.print(line); Serial.print("] Is underlined: "); Serial.println(isUnderlined);
            // Serial.print("Printed 1:"); Serial.print(isPrinted1);Serial.print(" Printed 2:"); Serial.print(isPrinted2);
            // Serial.println(isTransparent) ? " Transparent" : " Not Transparent");        
            graphics.drawText(col * graphics.settings.charWidth, line * graphics.settings.charHeight, character, color, bgColor, !isTransparent);
            
        }
    }
    return modified;
}

// void GPU::ClearMouseObject(GraphicsObject2D *obj)
// {
//     Serial.print("Clearing Mouse object on bank ");Serial.print(__activeBank); Serial.print(" at "); Serial.print(obj->shape->vertecies[0].x); Serial.print(", "); Serial.println(obj->shape->vertecies[0].y);

//     auto objBounds = obj->getBounds();
//     int bufferOffset = __activeBank * objBounds.height() * objBounds.width() * 2;
//     if(__activeBank ? obj->drawnOnMem1 : obj->drawnOnMem2){
//         //write back if bank data should be there
//         graphics.drawBuffer(
//             objBounds.x1(), 
//             objBounds.y1(), 
//             objBounds.width(),
//             objBounds.height(), 
//             obj->texture->colors + bufferOffset
//         );
//     }
    
//     if(__activeBank)
//         obj->drawnOnMem2 = false;
//     else
//         obj->drawnOnMem1 = false;
    
// }

// void GPU::DrawMouseObject(GraphicsObject2D *obj)
// {

//      Serial.print("Drawing Mouse object on bank ");Serial.print(__activeBank); Serial.print(" at ");  Serial.print(obj->shape->vertecies[0].x); Serial.print(", "); Serial.print(obj->shape->vertecies[0].y);

//     // if(obj->shape->numberOfVerticies > 1){
//     //     Serial.print("\t "); Serial.print(obj->shape->vertecies[1].x); Serial.print(", "); Serial.print(obj->shape->vertecies[1].y);
//     // }
//     Serial.println();

//     // buffer has 4 sections:
//     // 1. bank 1 before storage
//     // 2. bank 1 mouse cursor storage
//     // 3. bank 2 before storage
//     // 4. bank2 after storage
//     int16_t x1 = obj->shape->vertecies[0].x;
//     int16_t x2 = obj->shape->vertecies[1].x;
//     int16_t y1 = obj->shape->vertecies[0].y;
//     int16_t y2 = obj->shape->vertecies[1].y;
    
//     auto width = x2 - x1 ;
//     auto height = y2 - y1;
//     // Serial.print("Bounds: "); Serial.print(objBounds.x1());Serial.print(", "); Serial.print(objBounds.y1());    
//     // Serial.print(" - W x H: "); Serial.print(objBounds.x2() - objBounds.x1());Serial.print(", "); Serial.print(objBounds.y2() - objBounds.y1());

//     // Serial.print(" Verticies: "); Serial.print(x1);Serial.print(", "); Serial.print(y1);
//     // Serial.print(" - W x H: "); Serial.print(x2 - x1);Serial.print(", "); Serial.println(y2 - y1);
//     auto objBounds = obj->getBounds();
//     int bufferOldOffset = __activeBank * height * width * 2;
//     int bufferCursorOffset = (gpu.activeBank() * height * width * 2) + (height * width);
        
//     // Serial.print("Buffer old offset: "); Serial.println(bufferOldOffset);
//     // Serial.print("Buffer mouse offset: "); Serial.println(bufferCursorOffset);

//     //store to old
//     graphics.readBuffer(objBounds.x1(),objBounds.y1(),width,height,obj->texture->colors + bufferOldOffset);
//     //draw new
//     graphics.drawBuffer(objBounds.x1(),objBounds.y1(),width,height,obj->texture->colors + bufferCursorOffset);
    
    
//     if(__activeBank)
//         obj->drawnOnMem2 = true;
//     else
//         obj->drawnOnMem1 = true;
// }

void GPU::Draw2DObject(GraphicsObject2D* obj)
{
    // Serial.print("Drawing 2D object at "); Serial.print(obj->shape->vertecies[0].x); Serial.print(", "); Serial.println(obj->shape->vertecies[0].y);

    // if(obj->shape->numberOfVerticies > 1){
    //     Serial.print("\tv2 "); Serial.print(obj->shape->vertecies[1].x); Serial.print(", "); Serial.println(obj->shape->vertecies[1].y);
    // }

    // Serial.print("Shape type: "); Serial.print(ShapeName[ obj->shape->shape]);
    // Serial.print(" with color:");
    // Serial.println(obj->texture->colors[0]);
 
   

   uint8_t drawColor = obj->texture ? obj->texture->colors[0] : obj->color;

    

    if(obj->shape->shape == Line){
        graphics.drawLine(obj->shape->vertecies[0], obj->shape->vertecies[1], drawColor);
    }
    else if(obj->shape->shape == Triangle){
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawTriangle(
                obj->shape->vertecies[0].x, obj->shape->vertecies[0].y ,
                obj->shape->vertecies[1].x, obj->shape->vertecies[1].y,
                obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillTriangle(
                obj->shape->vertecies[0].x, obj->shape->vertecies[0].y ,
                obj->shape->vertecies[1].x, obj->shape->vertecies[1].y,
                obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,
                drawColor
            );
    }
    else if(obj->shape->shape == Rectangle){
       
        if(obj->shape->style == FillStyle::Outline){           
            graphics.drawRectangle(
                obj->shape->vertecies[0],
                obj->shape->vertecies[1],
                drawColor
            );
        }
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillRectangle(
                obj->shape->vertecies[0],
                obj->shape->vertecies[1],
                drawColor
            );
    }

    else if(obj->shape->shape == Circle){
        Circle2D* circle = static_cast<Circle2D*>(obj->shape);
        //Serial.print("Drawing circle at ("); Serial.print(circle->vertecies[0].x);  Serial.print(", "); Serial.print(circle->vertecies[0].y); Serial.print(") with radius ");  Serial.println(circle->radius);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawCircle(
                circle->vertecies[0].x,
                circle->vertecies[0].y,
                circle->radius,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillCircle(
                circle->vertecies[0].x,
                circle->vertecies[0].y,
                circle->radius,
                drawColor
            );
    }
    else if(obj->shape->shape == Oval){
        Oval2D* oval = static_cast<Oval2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawOval(
                oval->vertecies[0].x,
                oval->vertecies[0].y,
                oval->radiusX,
                oval->radiusY,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillOval(
                oval->vertecies[0].x,
                oval->vertecies[0].y,
                oval->radiusX,
                oval->radiusY,
                drawColor
            );
    }
    else if(obj->shape->shape == Arc){
        Arc2D* arc = static_cast<Arc2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawArc(
                arc->vertecies[0].x,
                arc->vertecies[0].y,
                arc->startDeg,
                arc->endDeg,
                arc->radius,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill){
        //    graphics.fillArc(
        //         arc->vertecies[0].x,
        //         arc->vertecies[0].y,
        //         arc->startDeg,
        //         arc->endDeg,
        //         arc->radius,
        //         obj->texture->colors[0]
        //     );
        }
        
    }

    if(obj->text){
        // auto textWidth = strlen(obj->text) * graphics.settings.charWidth;
        uint16_t textHeight = 0, textWidth = 0;
        for(int i=0; i < obj->shape->numberOfVerticies; i++){
            textWidth += obj->shape->vertecies[i].x;
            textHeight += obj->shape->vertecies[i].y;
        }
        auto xpos = (textWidth / obj->shape->numberOfVerticies) - ((strlen(obj->text) * graphics.settings.charWidth) / 2);
        auto ypos = (textHeight / obj->shape->numberOfVerticies) - graphics.settings.charHeight / 2;
        graphics.drawText(xpos, ypos, obj->text, obj->color ^ 0xFF, obj->color,true);
        //Serial.print("printing "); Serial.print(obj->text); Serial.print(" at ("); Serial.print(xpos);Serial.print(", "); Serial.print(ypos);;Serial.println(")");
    }

}

void GPU::ClearScreen()
{
    graphics.clear();
    ClearObjects();
}
void GPU::ClearScreen(uint8_t color){
    graphics.settings.backgroundColor = color;
    graphics.clear();
    ClearObjects();
    
}
void GPU::Set2DObjects(ShapeList<GraphicsObject2D>* list)
{
    // guard
    if(list == nullptr){
        // just clear our internal list
        if(_renderCanvas.shapeList){
            _renderCanvas.shapeList->clear();
        }
        return;
    }

    // move elements from external list into GPU's internal list so GPU owns them.
    // This prevents GPU from pointing into caller-owned memory that may be deleted.
    if(!_renderCanvas.shapeList) _renderCanvas.shapeList = new ShapeList<GraphicsObject2D>();

    _renderCanvas.shapeList->clear();
    for(auto &obj : *list){
        _renderCanvas.shapeList->push_back(std::move(obj));
    }
    list->clear();
}

void GPU::ClearObjects()
{
    _isBank1Initialized = false;
    _isBank2Initialized = false;

    if (_renderCanvas.shapeList) {
        if (_renderCanvas.shapeList->size() > 0) {
            _renderCanvas.shapeList->clear();  // this runs each GraphicsObject2D dtor
        }        
    }

    // reset text buffer content
    _textBuffer.Clear();

    #ifdef DEBUG_GPU
    // Optional: update RAM diagnostics
    PrintRam(Serial);
    #endif
}
#endif