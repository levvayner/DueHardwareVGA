#ifndef GRAPHICS2DOBJECT_H
#define GRAPHICS2DOBJECT_H
#include "Enums2D.h"
#include "Objects2D.h"

struct GraphicsObject2D{

    Shape2D* shape = nullptr;
    Texture2D* texture = nullptr;
    uint8_t color;
    bool drawnOnMem1 = false;
    bool drawnOnMem2 = false;

    //texture by pointer
    GraphicsObject2D(Shape2D* shape, Texture2D* texture){
        this->shape = shape;
        this->texture = texture;
    }
    //color only
    GraphicsObject2D(Shape2D* shape, uint8_t color){
        this->shape = shape;
        this->texture = nullptr;
        this->color = color;        
    }
    // copy constructor (shallow copy of pointers, copies flags)
    GraphicsObject2D(const GraphicsObject2D& o)
        : shape(o.shape),
          texture(o.texture),
          color(o.color),
          drawnOnMem1(o.drawnOnMem1),
          drawnOnMem2(o.drawnOnMem2)
    {}
    // disable accidental by-value copies (they deep-allocate)
    GraphicsObject2D(Shape2D* shape, Texture2D texture) = delete;
    GraphicsObject2D(Shape2D shape, Texture2D texture) = delete;


    // copy assignment (shallow copy)
    GraphicsObject2D& operator=(const GraphicsObject2D& o){
        if (this == &o) return *this;
        shape = o.shape;
        texture = o.texture;
        color = o.color;
        drawnOnMem1 = o.drawnOnMem1;
        drawnOnMem2 = o.drawnOnMem2;
        return *this;
    }

    // move constructor
    GraphicsObject2D(GraphicsObject2D&& o) noexcept
        : shape(o.shape),
          texture(o.texture),
          color(o.color),
          drawnOnMem1(o.drawnOnMem1),
          drawnOnMem2(o.drawnOnMem2)
    {
        o.shape = nullptr;
        o.texture = nullptr;
        o.color = 0;
        o.drawnOnMem1 = false;
        o.drawnOnMem2 = false;
        #ifdef DEBUG_GPU
        Serial.println("Graphics 2D copied");
        #endif
    }

    // move assignment
    GraphicsObject2D& operator=(GraphicsObject2D&& o) noexcept {
        if (this == &o) return *this;
        shape = o.shape;
        texture = o.texture;
        color = o.color;
        drawnOnMem1 = o.drawnOnMem1;
        drawnOnMem2 = o.drawnOnMem2;
        o.shape = nullptr;
        o.texture = nullptr;
        o.drawnOnMem1 = false;
        o.drawnOnMem2 = false;
        return *this;
    }
    ~GraphicsObject2D(){
        #ifdef DEBUG_GPU
        Serial.println("GraphicsObject2D destroyed");
        #endif
        if (shape != nullptr) {
            delete shape;
            shape = nullptr;
        }
        if (texture != nullptr){
            delete texture;
            texture = nullptr;
        }
    }
};
#endif