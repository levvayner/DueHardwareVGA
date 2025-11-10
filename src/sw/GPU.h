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

        /// @brief Should be called in loop to render screen
        /// @return Returns true if any rendering was done. False otherwise 
        bool Render();
        /// @brief Invalidate both banks forcing screen to clear and all objects to be redrawn
        void Invalidate();
        void Invalidate(Rectangle2D * bounds);
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
        Graphics2DObject* Add2DObject(const Graphics2DObject& obj);    
        void Set2DObjects(ShapeList<Graphics2DObject>* list);
        void Set3DObjects(ShapeList<Graphics3DObject>* list);
        void ClearObjects();

        /// Get the list of 2D objects
        ShapeList<Graphics2DObject>*  Get2DObjects(){
            return _renderCanvas2D.shapeList;        
        }
        
        //hand over control to GPU for objects
        void Set2DObjects(ShapeList<Graphics2DObject>** list){
            _renderCanvas2D.shapeList = *list;        
        }

        /// Get the list of 3D objects
        ShapeList<Graphics3DObject>*  Get3DObjects(){
            return _renderCanvas3D.shapeList;        
        }
        //hand over control to GPU for objects
        void Set3DObjects(ShapeList<Graphics3DObject>** list){
            _renderCanvas3D.shapeList = *list;        
        }

        Graphics2DObject* Get2DObjectAt(int16_t x, int16_t y ){
            for(auto &o : *_renderCanvas2D.shapeList){
                //Serial.print("Checking object at "); Serial.print(o.shape->vertices[0].x); Serial.print(", "); Serial.println(o.shape->vertices[0].y);
                if(o.shape->vertices[0].x == x && o.shape->vertices[0].y == y){
                    //Serial.print("Found object at "); Serial.print(x); Serial.print(", "); Serial.println(y);
                    return &o;
                }
            }
            return nullptr;
        }
        Graphics3DObject* Get3DObjectAt(int16_t x, int16_t y , int16_t z ){
            for(auto &o : *_renderCanvas3D.shapeList){
                //Serial.print("Checking object at "); Serial.print(o.shape->vertices[0].x); Serial.print(", "); Serial.println(o.shape->vertices[0].y);
                if(o.shape->vertices[0].x == x && o.shape->vertices[0].y == y && o.shape->vertices[0].z == z){
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

        void SetMouseCanvas(ShapeList<Graphics2DObject>* mouseCursor){
            this->_mouseCanvas.shapeList = mouseCursor;
        }
        /// @brief Request GPU to force a render on the next call to Render
        /// @return True if request is accepted.
        inline bool RequestRender(){
            if(graphics.isWaiting())
                return false;

            
            _renderRequested = true;
            return true;
        }
        inline void ShowFPS(){
            if(_showFPS) return;
            
            _fpsCounter = new Graphics2DObject(new Rectangle2D(graphics.settings.screenWidth - 120, 10,graphics.settings.screenWidth - 10,30,Fill),Color::LIGHT_GREEN,"");
            _fpsCounter->color = Color::BLUE;
            _fpsCounter = _fpsCanvas.shapeList->push_back(std::move(*_fpsCounter));
            _showFPS = true;
        }
        inline void HideFPS(){
            if(!_showFPS) return;
            _fpsCanvas.shapeList->clear();
            Invalidate();
            _showFPS = false;
        }
        

    protected:
        
        void Draw2DObject(Graphics2DObject * obj);
        void Draw3DObject(Graphics3DObject* obj);
        bool DrawTextBuffer();

        int InvalidateObjects(int16_t x, int16_t y);
        int InvalidateObjects(int16_t x, int16_t y, int16_t z);
        int InvalidateObjects(Point2D location);
        int InvalidateObjects(Rectangle2D * bounds);

    private:
        void addFpsEntry();
    private:
        //VRAM* _graphics;
        RenderMode _renderMode;
        TextBuffer _textBuffer;
        //bool _activeBank = 0;
        Graphics2D _renderCanvas2D;
        Graphics3D _renderCanvas3D;
        Graphics2D _mouseCanvas;
        Graphics2D _fpsCanvas;
        Graphics2DObject * _fpsCounter;
        bool _isBank1Initialized = false;
        bool _isBank2Initialized = false;
        bool _renderRequested = false; // external components can ask gpu to force bank swap
        bool _showFPS = false;


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