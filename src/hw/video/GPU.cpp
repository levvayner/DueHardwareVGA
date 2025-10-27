#ifndef GPU_CPP
#define GPU_CPP

#include "GPU.h"
#include <stdlib.h>
#include <utility>

extern VRAM graphics;
extern volatile bool __isBufferReadySet;
volatile bool __activeBank;

extern char _end;
extern "C" char* sbrk(int i);

void clearReadySet(){
    __isBufferReadySet = false;
    __activeBank = !__activeBank;
    digitalWrite(PIN_READY,LOW); 
    //graphics.setReady(false);
    //Serial.print(millis()); Serial.println("            Cleared ready");
}



void GPU::saveRamStates()
{
 char* heapend = sbrk(0);
 register char* stack_ptr asm("sp");
 struct mallinfo mi = mallinfo();
//  if (MaxUsedStaticRAM < &_end - ramstart)
//  {
//  MaxUsedStaticRAM = &_end - ramstart;
//  }
//  if (MaxUsedHeapRAM < mi.uordblks)
//  {
//  MaxUsedHeapRAM = mi.uordblks;
//  }
//  if (MaxUsedStackRAM < ramend - stack_ptr)
//  {
//  MaxUsedStackRAM = ramend - stack_ptr;
//  }
 if (MinfreeRAM > stack_ptr - heapend + mi.fordblks || MinfreeRAM == 0)
 { 
 MinfreeRAM = stack_ptr - heapend + mi.fordblks;
 }
}

void GPU::PrintRAMstates()
{
//  Serial.print("Max Used RAM STATIC: ");
//  Serial.print(MaxUsedStaticRAM);
//  Serial.print(" HEAP: ");
//  Serial.print(MaxUsedHeapRAM);
//  Serial.print(" STACK: ");
//  Serial.print(MaxUsedStackRAM);
 Serial.print(" Min FREE RAM: ");
 Serial.println(MinfreeRAM);
}


GPU gpu(RenderMode::Buffered);
GPU::GPU(RenderMode mode = RenderMode::Direct)
{
    //graphics = &graphics;
    _graphics2D.shapeList = new ShapeList<GraphicsObject2D>();
    _renderMode = mode;
    graphics.begin();
    graphics.settings.backgroundColor = 0x00;
    attachInterrupt(digitalPinToInterrupt(PIN_BANK_SELECT), clearReadySet, CHANGE);
}

void GPU::Render()
{
    unsigned long startTime = millis();
    while(graphics.isWaiting() && (millis() - startTime < 40));
    //_activeBank = digitalRead(PIN_BANK_SELECT);
    #ifdef DEBUG_GPU
    Serial.print("Rendering to bank "); Serial.println(__activeBank);
    Serial.print("Objects to render: "); Serial.println(_graphics2D.shapeList->size());
    #endif
    if(__activeBank == 0 && !_isBank1Initialized){
        
        graphics.clear(0,0,1 << graphics.settings.horizontalBits, 1 << graphics.settings.horizontalBits); //clear full screen
        _isBank1Initialized = true;
        Serial.println("Initialized bank 0");
    }
    else if(__activeBank == 1 && !_isBank2Initialized){
        graphics.clear(0,0,1 << graphics.settings.horizontalBits, 1 << graphics.settings.horizontalBits);
        _isBank2Initialized = true;
        Serial.println("Initialized bank 1");
    }
    for(auto &obj : *_graphics2D.shapeList){
        if(__activeBank == 0 && !obj.drawnOnMem1){
            Draw2DObject(&obj);
            obj.drawnOnMem1 = true;
        }
        else if(__activeBank == 1 && !obj.drawnOnMem2){
            Draw2DObject(&obj);
            obj.drawnOnMem2 = true;
        }
    }    
    graphics.setReady();
}

bool GPU::activeBank()
{
    return __activeBank;
}

void GPU::Draw2DObject(GraphicsObject2D* obj)
{
    // Serial.print("Drawing 2D object at "); Serial.print(obj->shape->vertecies[0].x); Serial.print(", "); Serial.println(obj->shape->vertecies[0].y);
    // Serial.print("Shape type: "); Serial.print(obj->shape->shape);
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

}
void GPU::ClearScreen(){
    graphics.clear();
    ClearObjects();
    
}
void GPU::Set2DObjects(ShapeList<GraphicsObject2D>* list)
{
    // guard
    if(list == nullptr){
        // just clear our internal list
        if(_graphics2D.shapeList){
            _graphics2D.shapeList->clear();
        }
        return;
    }

    // move elements from external list into GPU's internal list so GPU owns them.
    // This prevents GPU from pointing into caller-owned memory that may be deleted.
    if(!_graphics2D.shapeList) _graphics2D.shapeList = new ShapeList<GraphicsObject2D>();

    _graphics2D.shapeList->clear();
    for(auto &obj : *list){
        _graphics2D.shapeList->push_back(std::move(obj));
    }
    list->clear();
}

void GPU::ClearObjects()
{
    _isBank1Initialized = false;
    _isBank2Initialized = false;
    // If there is no list, nothing to do
    if (_graphics2D.shapeList == nullptr) return;

    // clear() will call each GraphicsObject2D destructor (which should delete its shape/texture)
    _graphics2D.shapeList->clear();

    // Replace the container with a fresh empty one to ensure no dangling references remain.
    delete _graphics2D.shapeList;
    _graphics2D.shapeList = new ShapeList<GraphicsObject2D>();

    #ifdef DEBUG_GPU
    // Optional: update RAM diagnostics
    saveRamStates();
    PrintRAMstates();
    #endif
}
#endif