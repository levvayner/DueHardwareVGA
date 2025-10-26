#ifndef GPU_CPP
#define GPU_CPP

#include "GPU.h"
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
    
    //2D graphics
    // for(int idx=0;idx < graphics2D.objectCount; idx++){
    // //for( GraphicsObject2D obj : graphics2D.objects){
        
    //     if(_activeBank == 0 && !(graphics2D.objects+ idx)->drawnOnMem1){
    //         Draw2DObject((graphics2D.objects+ idx));
    //         (graphics2D.objects+ idx)->drawnOnMem1 = true;
    //     }
    //     else if(_activeBank == 1 && !(graphics2D.objects+ idx)->drawnOnMem2){
    //         (graphics2D.objects+ idx)->drawnOnMem2 = true;
    //     }
    // }
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
    if(obj->shape->shape == Line){
        graphics.drawLine(obj->shape->vertecies[0], obj->shape->vertecies[1],obj->texture->colors[0]);
    }
    else if(obj->shape->shape == Triangle){
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawTriangle(
                obj->shape->vertecies[0].x, obj->shape->vertecies[0].y ,
                obj->shape->vertecies[1].x, obj->shape->vertecies[1].y,
                obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillTriangle(
                obj->shape->vertecies[0].x, obj->shape->vertecies[0].y ,
                obj->shape->vertecies[1].x, obj->shape->vertecies[1].y,
                obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,
                obj->texture->colors[0]
            );
    }
    else if(obj->shape->shape == Rectangle){
        #ifdef DEBUG_GPU
        Serial.print("Drawing rectangle\n");
        Serial.print("Top left: (");
        Serial.print(obj->shape->vertecies[0].x);
        Serial.print(", ");
        Serial.print(obj->shape->vertecies[0].y);
        Serial.print("), Bottom right: (");
        Serial.print(obj->shape->vertecies[1].x);
        Serial.print(", ");
        Serial.print(obj->shape->vertecies[1].y);
        Serial.print(")");
        Serial.print(" with color:");
        Serial.println(obj->texture->colors[0]);
        #endif
        if(obj->shape->style == FillStyle::Outline){           
            graphics.drawRectangle(
                obj->shape->vertecies[0],
                obj->shape->vertecies[1],
                obj->texture->colors[0]
            );
        }
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillRectangle(
                obj->shape->vertecies[0],
                obj->shape->vertecies[1],
                obj->texture->colors[0]
            );
    }

    else if(obj->shape->shape == Circle){
        Circle2D* circle = static_cast<Circle2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            graphics.drawCircle(
                circle->vertecies[0].x,
                circle->vertecies[0].y,
                circle->radius,
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillCircle(
                circle->vertecies[0].x,
                circle->vertecies[0].y,
                circle->radius,
                obj->texture->colors[0]
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
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            graphics.fillOval(
                oval->vertecies[0].x,
                oval->vertecies[0].y,
                oval->radiusX,
                oval->radiusY,
                obj->texture->colors[0]
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
                obj->texture->colors[0]
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
    //TODO: add polygon support

    // if(Line2D* lPtr = dynamic_cast<Line2D*>(obj->shape)){
    //     graphics.drawLine(lPtr->p1(), obj->shape->vertecies[1],obj->texture->colors[0]);
    // }
    // else if(Line2D* lPtr = dynamic_cast<Line2D*>(obj->shape)){
    // }
    // else if(Triangle2D* lPtr = dynamic_cast<Triangle2D*>(obj->shape)){
    //     if(obj->shape->style == FillStyle::Outline)
    //         graphics.drawTriangle(lPtr->p1().x, lPtr->p1().y , obj->shape->vertecies[1].x, obj->shape->vertecies[1].y, obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,obj->texture->colors[0]);
    //     else if(obj->shape->style == FillStyle::Fill){
    //         graphics.fillTriangle(lPtr->p1().x, lPtr->p1().y , obj->shape->vertecies[1].x, obj->shape->vertecies[1].y, obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,obj->texture->colors[0]);
    //     }

    // }
    // else if(Rectangle2D* lPtr = dynamic_cast<Rectangle2D*>(obj->shape)){
    // }
    // else if(Circle2D* lPtr = dynamic_cast<Circle2D*>(obj->shape)){
    // }
    // else if(Oval2D* lPtr = dynamic_cast<Oval2D*>(obj->shape)){
    // }
    // else if(Arc2D* lPtr = dynamic_cast<Arc2D*>(obj->shape)){
    // }
    // else if(Line2D* lPtr = dynamic_cast<Line2D*>(obj->shape)){
    // }

    //Serial.println("Done drawing object");
}
void GPU::ClearScreen(){
    graphics.clear();
    Clear2DObjects();
    _isBank1Initialized = false;
    _isBank2Initialized = false;
}
void GPU::Clear2DObjects(){     
            // while (_graphics2D.shapeList->size() > 0)
            // {
            //     _graphics2D.shapeList->pop_front();
            //     Serial.println("Popped front 2D object");
            //     Serial.print("Objects in list: "); Serial.print(_graphics2D.shapeList->size());
            // }
            _graphics2D.shapeList->for_each([](GraphicsObject2D& obj){
                free(obj.shape->vertecies);
                obj.shape = nullptr;
                free(obj.texture->colors);
                obj.texture = nullptr;                         
            });
            _graphics2D.shapeList->clear();   
            delete _graphics2D.shapeList;
            _graphics2D.shapeList = new ShapeList<GraphicsObject2D>();                     
        }
#endif