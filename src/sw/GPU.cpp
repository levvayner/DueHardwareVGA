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

void GPU::begin(uint16_t textBufferWidth, uint16_t textBufferHeight)
{
    _renderCanvas2D.shapeList = new ShapeList<Graphics2DObject>();
    _mouseCanvas.shapeList = new ShapeList<Graphics2DObject>();
    _fpsCanvas.shapeList =  new ShapeList<Graphics2DObject>();
    _renderCanvas3D.shapeList = new ShapeList<Graphics3DObject>();
    char buf[36];
    sprintf(buf,"Starting display with %d x %d resolution\n", graphics.settings.screenWidth, graphics.settings.screenHeight);
    Serial.println(buf);
    graphics.begin(graphics.settings.screenWidth, graphics.settings.screenHeight, graphics.settings.foregroundColor);
    _textBuffer = *new TextBuffer(textBufferWidth == 0 ? graphics.settings.screenWidth/graphics.settings.charWidth : textBufferWidth, textBufferHeight == 0 ? graphics.settings.screenHeight/graphics.settings.charHeight : textBufferHeight);
    
    attachInterrupt(digitalPinToInterrupt(PIN_BANK_SELECT), clearReadySet, CHANGE);
}

void GPU::end()
{
    ClearScreen();    
    detachInterrupt(digitalPinToInterrupt(PIN_BANK_SELECT));
}

bool GPU::Render()
{
    bool modified = false;
    unsigned long startTime = millis();
    while(graphics.isWaiting() && (millis() - startTime < 40));
    if(graphics.isWaiting()) {
        graphics.setReady(true);
        Serial.print("Skipping frame");
        return false;
    }
    if(_showFPS)
        addFpsEntry();
    //_activeBank = digitalRead(PIN_BANK_SELECT);
    #ifdef DEBUG_GPU
    Serial.print("Rendering to bank "); Serial.println(__activeBank);
    Serial.print("Objects to render: "); Serial.println(_renderCanvas2D.shapeList->size());
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

    if(_renderCanvas2D.shapeList != nullptr && _renderCanvas2D.shapeList->size() > 0){
        
        #ifdef DEBUG_GPU
            int rendered2DCount = 0;
        #endif
        for(auto &obj : *_renderCanvas2D.shapeList){
            if(__activeBank == 0 && !obj.drawnOnMem1){
                Draw2DObject(&obj);
                obj.drawnOnMem1 = true;
                modified = true;
                #ifdef DEBUG_GPU
                rendered2DCount++;
                #endif
            }
            else if(__activeBank == 1 && !obj.drawnOnMem2){
                Draw2DObject(&obj);
                obj.drawnOnMem2 = true;
                modified = true;
                #ifdef DEBUG_GPU
                rendered2DCount++;
                #endif
            } 
        }
        #ifdef DEBUG_GPU
        if(rendered2DCount > 0){
            Serial.print("Rendered ");Serial.print(rendered2DCount ); Serial.println(" 2D graphics objects");        
        }
        #endif
    } else{
        #ifdef DEBUG_GPU
        Serial.println("Not rendering 2D surface. No 2D objects found!");
        #endif
    }

    if(_renderCanvas3D.shapeList != nullptr && _renderCanvas3D.shapeList->size() > 0){
        
        #ifdef DEBUG_GPU
            int rendered3DCount = 0;
        #endif
        for(auto &obj : *_renderCanvas3D.shapeList){
            if(__activeBank == 0 && !obj.drawnOnMem1){
                Draw3DObject(&obj);
                obj.drawnOnMem1 = true;
                modified = true;
                #ifdef DEBUG_GPU
                rendered3DCount++;
                #endif
            }
            else if(__activeBank == 1 && !obj.drawnOnMem2){
                Draw3DObject(&obj);
                obj.drawnOnMem2 = true;
                modified = true;
                #ifdef DEBUG_GPU
                rendered3DCount++;
                #endif
            } 
        }
        #ifdef DEBUG_GPU
        if(rendered3DCount > 0){
            Serial.print("Rendered ");Serial.print(rendered3DCount ); Serial.println(" 3D graphics objects");        
        }
        #endif
    } else{
        #ifdef DEBUG_GPU
        Serial.println("Not rendering 3D surface. No 3D objects found!");
        #endif
    }

    //if(_renderMode == RenderMode::rmText){
        // in text mode, just clear the screen
        //graphics.clear();
        modified |= DrawTextBuffer();
    //}
    // else if(_renderMode == RenderMode::rmDirect){
    //     // in direct mode, draw all objects every frame
    //     for(auto &obj : *_renderCanvas2D.shapeList){
    //         Draw2DObject(&obj);
    //     }
    // }
        
   //}

    //fps canvas
    if(_fpsCanvas.shapeList != nullptr && _fpsCanvas.shapeList->size() > 0){
        #ifdef DEBUG_GPU
        Serial.print("Rendering fps ");Serial.print(_fpsCanvas.shapeList->size() ); Serial.println(" graphics objects");
        #endif
        for(auto &obj : *_fpsCanvas.shapeList){
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
    auto rendered = modified || _renderRequested;
    if(rendered)
        graphics.setReady();
    _renderRequested = false;
    //Serial.print("Rendering frame: "); Serial.print(millis() - startTime); Serial.println(" ms");
    return rendered;
}

void GPU::Invalidate()
{
    _isBank1Initialized = false;
    _isBank2Initialized = false;
    for(auto &obj : *_renderCanvas2D.shapeList){
        obj.invalidate();
    }
     for(auto &obj : *_renderCanvas3D.shapeList){
        obj.invalidate();
    }
    _textBuffer.Invalidate();
}

void GPU::Invalidate(Rectangle2D * bounds)
{
    graphics.clear(*bounds);
    InvalidateObjects(bounds);
    //TODO: invaldate text buffer part
}

bool GPU::activeBank()
{
    return __activeBank;
}

Graphics2DObject * GPU::Add2DObject(const Graphics2DObject &obj)
{
    return _renderCanvas2D.shapeList->push_back(std::move(obj));
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

int GPU::InvalidateObjects(Point2D location)
{
    return InvalidateObjects(location.x, location.y);
}

int GPU::InvalidateObjects(Rectangle2D * bounds)
{
    
    if(_renderCanvas2D.shapeList != nullptr && _renderCanvas2D.shapeList->size() > 0){
        for(auto & obj : *_renderCanvas2D.shapeList){
            auto intersects = bounds->intersects(obj.getBounds());
            if(intersects){
                //Serial.println("Invalidating bounds");
                obj.invalidate();
            }
        }
    }
    // if(_renderCanvas3D.shapeList != nullptr && _renderCanvas3D.shapeList->size() > 0){
    //     for(auto & obj : *_renderCanvas3D.shapeList){
    //         auto intersects = bounds->intersects(obj.getBounds());
    //         if(intersects){
    //             //Serial.println("Invalidating bounds");
    //             obj.invalidate();
    //         }
    //     }
    // }
    if(_mouseCanvas.shapeList != nullptr && _mouseCanvas.shapeList->size() > 0){
        for(auto & obj : *_renderCanvas2D.shapeList){
            auto intersects = bounds->intersects(obj.getBounds());
            if(intersects){
                obj.invalidate();
            }
        }
    }
    if(_fpsCanvas.shapeList != nullptr && _fpsCanvas.shapeList->size() > 0){
        for(auto & obj : *_fpsCanvas.shapeList){
            auto intersects = bounds->intersects(obj.getBounds());
            if(intersects){
                obj.invalidate();
            }
        }
    }
    return 0;
}

int GPU::InvalidateObjects(int16_t x, int16_t y)
{    
    for(auto & obj : *_renderCanvas2D.shapeList){
        if(obj.getBounds().contains(x,y))
        obj.invalidate();
    }
    for(auto & obj : *_mouseCanvas.shapeList){
        if(obj.getBounds().contains(x,y))
        obj.invalidate();
        
    }
    for(auto & obj : *_fpsCanvas.shapeList){
        if(obj.getBounds().contains(x,y))
        obj.invalidate();        
    }
    return 0;
}
/// @brief Invalidates objects that contain the point specified
/// @param x 
/// @param y 
/// @param z 
/// @return number of object invalidated
int GPU::InvalidateObjects(int16_t x, int16_t y, int16_t z)
{
    for(auto & obj : *_renderCanvas3D.shapeList){
        if(obj.getBounds().contains(x,y,z))
        obj.invalidate();
    }
   
    return 0;
}

unsigned long fpsTimes[2] = {0};
uint16_t fpsFrameCounter = 0;
double framesPerSecond = 0;
double samples = 20; //average over this many frames
void GPU::addFpsEntry()
{

    if(fpsFrameCounter == 0){
        fpsTimes[0] = millis();
    } else if(fpsFrameCounter == samples - 1){
        fpsTimes[1] = millis();
    }
    fpsFrameCounter++;
    if(fpsFrameCounter  == samples){
        char buf[12];
        //calculate and write out fps
        //shortcut, just compare first and last
        auto msPerFrame = ((double)(fpsTimes[1] - fpsTimes[0]) / (double)samples);
        framesPerSecond = 1000.0 / msPerFrame ;
        sprintf(buf,"FPS: %.2lf",framesPerSecond);
        _fpsCounter->update_text(buf);
        _fpsCounter->invalidate();
        //clear and reset counter
        memset(fpsTimes,0,sizeof(fpsTimes));
        fpsFrameCounter = 0;
        //update samples to have reading update about every second
        samples = framesPerSecond;
    }
}
void GPU::Draw2DObject(Graphics2DObject* obj)
{
    if(!Rectangle2D(0,0,graphics.settings.screenWidth, graphics.settings.screenHeight).intersects(obj->getBounds()))
        return;
    #if defined(DEBUG_GPU) && DEBUG_GPU > 0
    Serial.print("Drawing 2D object at "); Serial.print(obj->shape->vertices[0].x); Serial.print(", "); Serial.println(obj->shape->vertices[0].y);

    if(obj->shape->numberOfVerticies > 1){
        Serial.print("\tv2 "); Serial.print(obj->shape->vertices[1].x); Serial.print(", "); Serial.println(obj->shape->vertices[1].y);
    }

    Serial.print("Shape type: "); Serial.print(ShapeName[ obj->shape->shape]);
    Serial.print(" with color:");
    Serial.println(obj->texture->colors[0]);
    #endif
   

   uint8_t drawColor = obj->texture ? obj->texture->colors[0] : obj->color;

    

    if(obj->shape->shape == Line){
        graphics.drawLine(obj->shape->vertices[0], obj->shape->vertices[1], drawColor);
    }
    else if(obj->shape->shape == Triangle){
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawTriangle(
                obj->shape->vertices[0].x, obj->shape->vertices[0].y ,
                obj->shape->vertices[1].x, obj->shape->vertices[1].y,
                obj->shape->vertices[2].x,obj->shape->vertices[2].y,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillTriangle(
                obj->shape->vertices[0].x, obj->shape->vertices[0].y ,
                obj->shape->vertices[1].x, obj->shape->vertices[1].y,
                obj->shape->vertices[2].x,obj->shape->vertices[2].y,
                drawColor
            );
    }
    else if(obj->shape->shape == Rectangle){
       
        if(obj->shape->style == FillStyle::Outline){           
            graphics.drawRectangle(
                obj->shape->vertices[0],
                obj->shape->vertices[2],
                drawColor
            );
        }
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillRectangle(
                obj->shape->vertices[0],
                obj->shape->vertices[2],
                drawColor
            );
    }

    else if(obj->shape->shape == Circle){
        Circle2D* circle = static_cast<Circle2D*>(obj->shape);
        //Serial.print("Drawing circle at ("); Serial.print(circle->vertices[0].x);  Serial.print(", "); Serial.print(circle->vertices[0].y); Serial.print(") with radius ");  Serial.println(circle->radius);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawCircle(
                circle->vertices[0].x,
                circle->vertices[0].y,
                circle->radius,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillCircle(
                circle->vertices[0].x,
                circle->vertices[0].y,
                circle->radius,
                drawColor
            );
    }
    else if(obj->shape->shape == Oval){
        Oval2D* oval = static_cast<Oval2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawOval(
                oval->vertices[0].x,
                oval->vertices[0].y,
                oval->radiusX,
                oval->radiusY,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillOval(
                oval->vertices[0].x,
                oval->vertices[0].y,
                oval->radiusX,
                oval->radiusY,
                drawColor
            );
    }
    else if(obj->shape->shape == Arc){
        Arc2D* arc = static_cast<Arc2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawArc(
                arc->vertices[0].x,
                arc->vertices[0].y,
                arc->startDeg,
                arc->endDeg,
                arc->radius,
                drawColor
            );
            
        else if(obj->shape->style == FillStyle::Fill){
        //    graphics.fillArc(
        //         arc->vertices[0].x,
        //         arc->vertices[0].y,
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
            textWidth += obj->shape->vertices[i].x;
            textHeight += obj->shape->vertices[i].y;
        }
        auto xpos = (textWidth / obj->shape->numberOfVerticies) - ((strlen(obj->text) * graphics.settings.charWidth) / 2);
        auto ypos = (textHeight / obj->shape->numberOfVerticies) - graphics.settings.charHeight / 2;
        graphics.drawText(xpos, ypos, obj->text, obj->color ^ 0xFF, obj->color,true);
        //Serial.print("printing "); Serial.print(obj->text); Serial.print(" at ("); Serial.print(xpos);Serial.print(", "); Serial.print(ypos);;Serial.println(")");
    }

}

void GPU::Draw3DObject(Graphics3DObject* obj)
{
    if(!Rectangle2D(0,0,graphics.settings.screenWidth, graphics.settings.screenHeight).contains(obj->getProjectedBounds()))
        return;
    #if defined(DEBUG_GPU) && DEBUG_GPU > 0
    
    Serial.print("Drawing 3D object at "); Serial.print(obj->projected->vertices[0].x); Serial.print(", "); Serial.print(obj->projected->vertices[0].y);

    if(obj->projected->numberOfVerticies > 1){
        Serial.print(" - "); Serial.print(obj->projected->vertices[1].x); Serial.print(", "); Serial.println(obj->projected->vertices[1].y);
    }

    Serial.print("Shape type: "); Serial.print(ShapeName[ obj->projected->shape]);
    Serial.print(" with color:");
    Serial.print(obj->texture->colors[0]);
    Serial.println();
    #endif
   

   uint8_t drawColor = obj->texture ? obj->texture->colors[0] : obj->color;

    

    if(obj->projected->shape == Line){
        graphics.drawLine(obj->projected->vertices[0], obj->projected->vertices[1], drawColor);
    }
    else if(obj->projected->shape == Triangle){
        if(obj->projected->style == FillStyle::Outline)
            for(int idx = 0; idx < obj->projected->numberOfVerticies; idx++){
            auto nextIdx = (idx + 1) % obj->projected->numberOfVerticies;
            graphics.drawLine(obj->projected->vertices[idx], obj->projected->vertices[nextIdx], obj->color);            
        }
            // graphics.drawTriangle(
            //     obj->projected->vertices[0].x, obj->projected->vertices[0].y ,
            //     obj->projected->vertices[1].x, obj->projected->vertices[1].y,
            //     obj->projected->vertices[2].x,obj->projected->vertices[2].y,
            //     drawColor
            // );
            
        else if(obj->projected->style == FillStyle::Fill)
            graphics.fillTriangle(
                obj->projected->vertices[0].x, obj->projected->vertices[0].y ,
                obj->projected->vertices[1].x, obj->projected->vertices[1].y,
                obj->projected->vertices[2].x,obj->projected->vertices[2].y,
                drawColor
            );
    }
    else if(obj->projected->shape == Rectangle){
       
        if(obj->projected->style == FillStyle::Outline){           
         
            for(int idx = 0; idx < obj->projected->numberOfVerticies; idx++){
                auto nextIdx = (idx + 1) % obj->projected->numberOfVerticies;
                graphics.drawLine(obj->projected->vertices[idx], obj->projected->vertices[nextIdx], obj->color);            
            }
            // graphics.drawRectangle(
            //     obj->projected->vertices[0],
            //     obj->projected->vertices[2],
            //     drawColor
            // );
        }
        else if(obj->projected->style == FillStyle::Fill)
            graphics.fillRectangle(
                obj->projected->vertices[0],
                obj->projected->vertices[2],
                drawColor
            );
    }

    // else if(obj->projected->shape == Circle){
    //     Circle2D* circle = static_cast<Circle2D*>(obj->projected);
    //     //Serial.print("Drawing circle at ("); Serial.print(circle->vertices[0].x);  Serial.print(", "); Serial.print(circle->vertices[0].y); Serial.print(") with radius ");  Serial.println(circle->radius);
    //     if(obj->projected->style == FillStyle::Outline)
    //         graphics.drawCircle(
    //             circle->vertices[0].x,
    //             circle->vertices[0].y,
    //             circle->radius,
    //             drawColor
    //         );
            
    //     else if(obj->projected->style == FillStyle::Fill)
    //         graphics.fillCircle(
    //             circle->vertices[0].x,
    //             circle->vertices[0].y,
    //             circle->radius,
    //             drawColor
    //         );
    // }
    // else if(obj->projected->shape == Oval){
    //     Oval2D* oval = static_cast<Oval2D*>(obj->projected);
    //     if(obj->projected->style == FillStyle::Outline)
    //         graphics.drawOval(
    //             oval->vertices[0].x,
    //             oval->vertices[0].y,
    //             oval->radiusX,
    //             oval->radiusY,
    //             drawColor
    //         );
            
    //     else if(obj->projected->style == FillStyle::Fill)
    //         graphics.fillOval(
    //             oval->vertices[0].x,
    //             oval->vertices[0].y,
    //             oval->radiusX,
    //             oval->radiusY,
    //             drawColor
    //         );
    // }
    // else if(obj->projected->shape == Arc){
    //     Arc2D* arc = static_cast<Arc2D*>(obj->projected);
    //     if(obj->projected->style == FillStyle::Outline)
    //         graphics.drawArc(
    //             arc->vertices[0].x,
    //             arc->vertices[0].y,
    //             arc->startDeg,
    //             arc->endDeg,
    //             arc->radius,
    //             drawColor
    //         );
            
    //     else if(obj->projected->style == FillStyle::Fill){
    //     //    graphics.fillArc(
    //     //         arc->vertices[0].x,
    //     //         arc->vertices[0].y,
    //     //         arc->startDeg,
    //     //         arc->endDeg,
    //     //         arc->radius,
    //     //         obj->texture->colors[0]
    //     //     );
    //     }
        
    // }

    if(obj->text){
        // auto textWidth = strlen(obj->text) * graphics.settings.charWidth;
        uint16_t textHeight = 0, textWidth = 0;
        for(int i=0; i < obj->projected->numberOfVerticies; i++){
            textWidth += obj->projected->vertices[i].x;
            textHeight += obj->projected->vertices[i].y;
        }
        auto xpos = (textWidth / obj->projected->numberOfVerticies) - ((strlen(obj->text) * graphics.settings.charWidth) / 2);
        auto ypos = (textHeight / obj->projected->numberOfVerticies) - graphics.settings.charHeight / 2;
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
void GPU::Set2DObjects(ShapeList<Graphics2DObject>* list)
{
    // guard
    if(list == nullptr){
        // just clear our internal list
        if(_renderCanvas2D.shapeList){
            _renderCanvas2D.shapeList->clear();
        }
        return;
    }

    // move elements from external list into GPU's internal list so GPU owns them.
    // This prevents GPU from pointing into caller-owned memory that may be deleted.
    if(!_renderCanvas2D.shapeList) _renderCanvas2D.shapeList = new ShapeList<Graphics2DObject>();

    _renderCanvas2D.shapeList->clear();
    for(auto &obj : *list){
        _renderCanvas2D.shapeList->push_back(std::move(obj));
    }
    list->clear();
}

void GPU::Set3DObjects(ShapeList<Graphics3DObject> *list)
{
     // guard
    if(list == nullptr){
        // just clear our internal list
        if(_renderCanvas3D.shapeList){
            _renderCanvas3D.shapeList->clear();
        }
        return;
    }

    // move elements from external list into GPU's internal list so GPU owns them.
    // This prevents GPU from pointing into caller-owned memory that may be deleted.
    if(!_renderCanvas3D.shapeList) _renderCanvas3D.shapeList = new ShapeList<Graphics3DObject>();

    _renderCanvas3D.shapeList->clear();
    for(auto &obj : *list){
        _renderCanvas3D.shapeList->push_back(std::move(obj));
    }
    list->clear();
}

void GPU::ClearObjects()
{
    _isBank1Initialized = false;
    _isBank2Initialized = false;


    if (_renderCanvas2D.shapeList) {
        if (_renderCanvas3D.shapeList->size() > 0) {
            _renderCanvas3D.shapeList->clear();  // this runs each Graphics2DObject dtor
        }        
    }

    if (_renderCanvas3D.shapeList) {
        if (_renderCanvas3D.shapeList->size() > 0) {
            _renderCanvas3D.shapeList->clear();  // this runs each Graphics3DObject dtor
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


