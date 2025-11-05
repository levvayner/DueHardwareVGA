#ifndef GPU_H
#define GPU_H
#include "hw/video/VRAM.h"
#include <malloc.h>
#include "Text/TextBuffer.h"
#include <stdlib.h>
#include <utility>

#define CRITICAL_MEMORY_RESTART_THRESHOLD 512

enum RenderMode{
    rmText = 0,
    rmDirect = 1,
    rmBuffered = 2
};

class GPU
{
    public:
        GPU(RenderMode mode);
        void begin(uint16_t textBufferWidth = 0,  uint16_t textBufferHeight = 0);
        void end();

        void Render();
        /// @brief Invalidate both banks forcing screen to clear and all objects to be redrawn
        void Invalidate();
        void ClearScreen();
        void ClearScreen(uint8_t color);

        //void saveRamStates();
        //void PrintRAMstates();
        void PrintRam(Print &print);
        bool activeBank();

        TextBuffer* GetTextBuffer(){
            return &_textBuffer;
        }

        /// @brief Add a 2D object to the GPU
        /// @param obj The 2D object to add
        void Add2DObject(const GraphicsObject2D& obj);    
        void Set2DObjects(ShapeList<GraphicsObject2D>* list);
        void ClearObjects();

        /// Get the list of 2D objects
        ShapeList<GraphicsObject2D>*  Get2DObjects(){
            return _renderCanvas.shapeList;        
        }
        //hand over control to GPU for objects
        void Set2DObjects(ShapeList<GraphicsObject2D>** list){
            _renderCanvas.shapeList = *list;        
        }

        GraphicsObject2D* Get2DObjectAt(int16_t x, int16_t y ){
            for(auto &o : *_renderCanvas.shapeList){
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

        void SetRenderMode(RenderMode renderMode){
            _renderMode = renderMode;
        }

        void SetMouseCanvas(ShapeList<GraphicsObject2D>* mouseCursor){
            this->_mouseCanvas.shapeList = mouseCursor;
        }
        inline bool RequestRender(){
            if(graphics.isWaiting())
                return false;

            
            _renderRequested = true;
            return true;
        }
        

    protected:
        
        void Draw2DObject(GraphicsObject2D * shape);
        bool DrawTextBuffer();
    private:
        //VRAM* _graphics;
        RenderMode _renderMode;
        TextBuffer _textBuffer;
        //bool _activeBank = 0;
        Graphics2D _renderCanvas;
        Graphics2D _mouseCanvas;
        bool _isBank1Initialized = false;
        bool _isBank2Initialized = false;
        bool _renderRequested = false; // external components can ask gpu to force bank swap


        unsigned long previousMillisStatePrint;
        unsigned long WaitingTimeStatePrint = 1000;
        uint32_t MaxUsedHeapRAM;
        uint32_t MaxUsedStackRAM;
        uint32_t MaxUsedStaticRAM;
        uint32_t MinfreeRAM;
        
        char* ramstart = (char*)0x20070000;
        char* ramend = (char*)0x20088000;
};

extern GPU gpu;
#endif