#ifndef GPU_H
#define GPU_H
#include "VRAM.h"
#include <malloc.h>


enum RenderMode{
    Direct = 0,
    Buffered = 1
};

class GPU
{
    public:
        GPU(RenderMode mode);
        void Render();
        void ClearScreen();
        void saveRamStates();
        void PrintRAMstates();
        bool activeBank();

        /// @brief Add a 2D object to the GPU
        /// @param obj The 2D object to add
        void Add2DObject(const GraphicsObject2D& obj){
            // if(_graphics2D.shapeList == nullptr){
            //     _graphics2D.shapeList = new ShapeList<GraphicsObject2D>();
            // }   
            Serial.println(_graphics2D.shapeList->size());
            Serial.print(obj.shape->vertecies[0].x); Serial.print(", "); Serial.println(obj.shape->vertecies[0].y);
            _graphics2D.shapeList->push_back(obj);
        }
        // void Add2DObject(Shape2D shape, Texture2D texture){
        //     _graphics2D.shapeList->push_back(*new GraphicsObject2D(shape, texture));
        //     Serial.println(_graphics2D.shapeList->size());
        //     Serial.print(shape.vertecies[0].x); Serial.print(", "); Serial.println(shape.vertecies[0].y);
        // }

        void Set2DObjects(ShapeList<GraphicsObject2D>* list);
        void ClearObjects();

        /// Get the list of 2D objects
        ShapeList<GraphicsObject2D>*  Get2DObjects(){
            return _graphics2D.shapeList;        
        }
        void Set2DObjects(ShapeList<GraphicsObject2D>** list){
            _graphics2D.shapeList = *list;        
        }

        GraphicsObject2D* Get2DObjectAt(int16_t x, int16_t y ){
            for(auto &o : *_graphics2D.shapeList){
                //Serial.print("Checking object at "); Serial.print(o.shape->vertecies[0].x); Serial.print(", "); Serial.println(o.shape->vertecies[0].y);
                if(o.shape->vertecies[0].x == x && o.shape->vertecies[0].y == y){
                    //Serial.print("Found object at "); Serial.print(x); Serial.print(", "); Serial.println(y);
                    return &o;
                }
            }
            return nullptr;
        }

        VRAMSettings GetSettings(){
            return graphics.settings;
        }
        VRAM* GetGraphics(){
            return &graphics;
        }

    protected:
        void Draw2DObject(GraphicsObject2D* shape);
    private:
        //VRAM* _graphics;
        RenderMode _renderMode;
        //bool _activeBank = 0;
        Graphics2D _graphics2D;
        bool _isBank1Initialized = false;
        bool _isBank2Initialized = false;


        unsigned long previousMillisStatePrint;
        unsigned long WaitingTimeStatePrint = 1000;
        uint32_t MaxUsedHeapRAM;
        uint32_t MaxUsedStackRAM;
        uint32_t MaxUsedStaticRAM;
        uint32_t MinfreeRAM;
        
        char* ramstart = (char*)0x20070000;
        char* ramend = (char*)0x20088000;

};
#endif