#ifndef GRAPHICS3DOBJECT_H
#define GRAPHICS3DOBJECT_H
#include "Objects3D.h"
#include "Graphics3D.h"

struct Graphics3DObject{

    Shape3D* shape = nullptr;
    Shape2D* projected = nullptr;
    Texture2D* texture = nullptr;
    char* text = nullptr;
    uint8_t color;
    bool drawnOnMem1 = false;
    bool drawnOnMem2 = false;

    //texture by pointer
    Graphics3DObject(Shape3D* shape, Texture2D* texture, const char* text = ""){
        this->shape = shape;         
        this->texture = texture;
        this->drawnOnMem1 = false;
        this->drawnOnMem2 = false;
        auto len = strlen(text);  
        if(len > 0){
            this->text = new char[len + 1] {0};
            memcpy(this->text,text,len);
        }        
    }
    //color only
    Graphics3DObject(Shape3D* shape, uint8_t color, const char* text = ""){
        this->shape = shape;
        this->texture = nullptr;
        this->color = color;      
        this->drawnOnMem1 = false;
        this->drawnOnMem2 = false;
        auto len = strlen(text);  
        if(len > 0){
            this->text = new char[len + 1] {0};
            memcpy(this->text,text,len);
        }
    }
    // copy constructor (shallow copy of pointers, copies flags)
    Graphics3DObject(const Graphics3DObject& o)
        : shape(o.shape),
          projected(o.projected),
          texture(o.texture),
          text(o.text),
          color(o.color),
          drawnOnMem1(o.drawnOnMem1),
          drawnOnMem2(o.drawnOnMem2)          
    {}
    // disable accidental by-value copies (they deep-allocate)
    Graphics3DObject(Shape3D* shape, Texture2D texture) = delete;
    Graphics3DObject(Shape3D shape, Texture2D texture) = delete;


    // copy assignment (shallow copy)
    Graphics3DObject& operator=(const Graphics3DObject& o){
        if (this == &o) return *this;
        shape = o.shape;
        projected = o.projected;
        texture = o.texture;
        text = o.text;
        color = o.color;
        drawnOnMem1 = o.drawnOnMem1;
        drawnOnMem2 = o.drawnOnMem2;
        
        return *this;
    }

    // move constructor
    Graphics3DObject(Graphics3DObject&& o) noexcept
        : shape(o.shape),
          projected(o.projected),
          texture(o.texture),
          text(o.text),
          color(o.color),
          drawnOnMem1(o.drawnOnMem1),
          drawnOnMem2(o.drawnOnMem2)
          
    {
        o.shape = nullptr;
        o.projected = nullptr;
        o.texture = nullptr;
        o.color = 0;
        o.drawnOnMem1 = false;
        o.drawnOnMem2 = false;
        o.text = nullptr;
        #ifdef DEBUG_GPU
        Serial.println("Graphics 3D copied");
        #endif
    }

    // move assignment
    Graphics3DObject& operator=(Graphics3DObject&& o) noexcept {
        if (this == &o) return *this;
        shape = o.shape;
        projected = o.projected;
        texture = o.texture;
        text = o.text;
        color = o.color;
        drawnOnMem1 = o.drawnOnMem1;
        drawnOnMem2 = o.drawnOnMem2;
                
        o.shape = nullptr;
        o.projected = nullptr;
        o.texture = nullptr;
        o.text = nullptr;
        o.drawnOnMem1 = false;
        o.drawnOnMem2 = false;        
        
        return *this;
    }
    ~Graphics3DObject(){
        #ifdef DEBUG_GPU
        Serial.println("Graphics3DObject destroyed");
        #endif
        if (shape != nullptr) {
            delete shape;
            shape = nullptr;
        }
        if (projected != nullptr) {
            delete projected;
            projected = nullptr;
        }
        if (texture != nullptr){
            delete texture;
            texture = nullptr;
        }
        if (text != nullptr){
            delete text;
            text = nullptr;
        }
    }

    Rectangle3D getBounds(){
        int16_t minX = shape->vertices[0].x, maxX = shape->vertices[0].x;
        int16_t minY = shape->vertices[0].y, maxY = shape->vertices[0].y;
        int16_t minZ = shape->vertices[0].z, maxZ = shape->vertices[0].z;
        for(int idx=0; idx < shape->numberOfVerticies;idx++){
            auto verticieX = shape->vertices[idx].x;
            auto verticieY = shape->vertices[idx].y;
            auto verticieZ = shape->vertices[idx].z;
            if(minX > verticieX)
                minX = verticieX;
            if(minY > verticieY)
                minY = verticieY;
            if(minZ > verticieZ)
                minZ = verticieZ;

            if(maxX < verticieX)
                maxX = verticieX;
            if(maxY < verticieY)
                maxY = verticieY;
            if(maxZ < verticieZ)
                maxZ = verticieZ;
                

        }
        return Rectangle3D(minX,minY, maxX,maxY,minZ,maxZ);
    }

    Rectangle2D getProjectedBounds(){
        int16_t minX = projected->vertices[0].x, maxX = projected->vertices[0].x;
        int16_t minY = projected->vertices[0].y, maxY = projected->vertices[0].y;
        
        for(int idx=0; idx < projected->numberOfVerticies;idx++){
            auto verticieX = projected->vertices[idx].x;
            auto verticieY = projected->vertices[idx].y;
            if(minX > verticieX)
                minX = verticieX;
            if(minY > verticieY)
                minY = verticieY;

            if(maxX < verticieX)
                maxX = verticieX;
            if(maxY < verticieY)
                maxY = verticieY; 

        }
        return Rectangle2D(minX,minY, maxX,maxY);
    }
    //TODO: implement intersects method for all shapes
    
    void update_text(const char* text){
        if(this->text != nullptr)
            delete this->text;
        auto len = strlen(text);  
        if(len > 0){
            this->text = new char[len + 1] {0};
            memcpy(this->text,text,len);
        }
        this->invalidate();
    }

    void move_to(int16_t x, int16_t y, int16_t z){
        move_by(x - shape->vertices[0].x, y - shape->vertices[0].y, z - shape->vertices[0].z);       
    }

    void move_by(int16_t deltaX, int16_t deltaY, int16_t deltaZ){
        if(deltaX == 00 && deltaY == 0 && deltaZ == 0) return;
        Serial.print("Updating "); Serial.print(shape->numberOfVerticies); Serial.println(" verticies");
        Serial.print("Moving "); Serial.print(shape->vertices[0].x); Serial.print(", "); Serial.print(shape->vertices[0].y); Serial.print(", "); Serial.print(shape->vertices[0].z);
        Serial.print(" to "); Serial.print(shape->vertices[0].x + deltaX); Serial.print(", "); Serial.print(shape->vertices[0].y + deltaY);Serial.print(", "); Serial.println(shape->vertices[0].z + deltaZ);
        
        for(int idx=0; idx < shape->numberOfVerticies;idx++){
            shape->vertices[idx].x += deltaX;
            shape->vertices[idx].y += deltaY;
            shape->vertices[idx].z += deltaZ;
        }
        invalidate();
    }

    void invalidate(){
        this->drawnOnMem1 = false;
        this->drawnOnMem2 = false;
    }

    
};
#endif