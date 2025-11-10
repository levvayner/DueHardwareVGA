#ifndef OBJECTS3D_H
#define OBJECTS3D_H
#include "Arduino.h"

#include <sw/Color.h>
#include <sw/2D/Enums2D.h>
#include <sw/2D/Objects2D.h>
struct Point3D : public Point2D {
    int16_t z;
    Point3D(): z(0){
        x = 0;
        y = 0;
    };
    Point3D(int16_t x, int16_t y, int16_t z){ this->x = x; this->y = y; this->z = z;}
    Point3D(const Point3D& other){
        this->x = other.x;
        this->y = other.y;    
        this->z = other.z;    
    }

    Point3D& operator=(const Point2D& other) {
        if (this->x == other.x && this->y == other.y) return *this;
        this->x = other.x;
        this->y = other.y;        
        this->z = 0;
        return *this;
    }

    //copy
    

    Point3D& operator=(Point2D&& o) noexcept {
        if (this->x == o.x && this->y == o.y) return *this;
        this->x = o.x;
        this->y = o.y; 
        this->z = 0;       
        return *this;
    }

    Point3D& operator=(const Point3D& other) {
        if (this->x == other.x && this->y == other.y) return *this;
        this->x = other.x;
        this->y = other.y;    
        this->z = other.z;     
        return *this;
    }

    Point3D& operator=(Point3D&& o) noexcept {
        if (this->x == o.x && this->y == o.y) return *this;
        this->x = o.x;
        this->y = o.y; 
        this->z = o.z;            
        return *this;
    }

    bool operator ==(Point3D& other){
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator !=(Point3D& other){
        return x != other.x || y != other.y || z != other.z;
    }
};



struct Shape3D : public Shape2D {
    public:
    uint8_t numberOfVerticies;
    Point3D* vertices;
    FillStyle style = FillStyle::Outline; // e.g. 0 is outline, 1 is fill, 2 is vertical gradient...
    //TODO: dynamically find shape like dynamic_cast
    Shape shape;
    virtual ~Shape3D() { delete[] vertices; }  // delete[] (matches new[])
    
    virtual bool operator==(Shape3D& other){
        return numberOfVerticies == other.numberOfVerticies &&
        style == other.style &&
        shape == other.shape;
        // TODO: iterate and compare
    }

    void move_to(int16_t x, int16_t y, int16_t z){
        move_by(x - vertices[0].x, y - vertices[0].z, y - vertices[0].z);       
    }

    void move_by(int16_t deltaX, int16_t deltaY, int16_t deltaZ){
        // Serial.print("Updating "); Serial.print(numberOfVerticies); Serial.println(" verticies");
        // Serial.print("Moving "); Serial.print(deltaX); Serial.print(", "); Serial.print(deltaY);
        // Serial.print(" to "); Serial.print(vertices[0].x + deltaX); Serial.print(", "); Serial.println(vertices[0].y + deltaY);
        
        for(int idx=0; idx < numberOfVerticies;idx++){
            vertices[idx].x += deltaX;
            vertices[idx].y += deltaY;
        }
    }

    void setFillStyle(FillStyle style){
        this->style = style;
    }
};

// struct Circle2D : public Shape2D{
//     public:
//     uint16_t radius;
//     Circle2D(int16_t x, int16_t y, uint16_t radius, FillStyle style = FillStyle::Outline){
//         numberOfVerticies = 1;
//         shape = Circle;
//         vertices = new Point2D[1]; 
//         vertices[0] = Point2D(x, y);
//         this->style = style;
//         //vertices.push_back(Point2D(x, y));
//         this->radius = radius;
//     }
// };

// struct Oval2D : public Shape2D{
//     public:
//     uint16_t radiusX;
//     uint16_t radiusY;
//     Oval2D(int16_t x, int16_t y, uint16_t radiusX, uint16_t radiusY, FillStyle style = FillStyle::Outline){
//         numberOfVerticies = 1;
//         shape = Oval;
//         this->style = style;        
//         vertices = new Point2D[1]; 
//         vertices[0] = Point2D(x, y);
//         this->radiusX = radiusX;
//         this->radiusY = radiusY;
//     }
// };

// struct Arc2D : public Shape2D{
//     public:
//     uint16_t radius;
//     uint8_t startDeg;
//     uint8_t endDeg;
//     Arc2D(int16_t x, int16_t y, uint16_t radius, uint8_t startDeg, uint8_t endDeg, FillStyle style = FillStyle::Outline){
//         this->numberOfVerticies = 1;
//         shape = Arc;
//         this->style = style;
//         vertices = new Point2D[2]; 
//         vertices[0] = Point2D(x, y);
//         this->radius = radius;
//         this->startDeg = startDeg;
//         this->endDeg = endDeg; 
//     }
// };


// struct Line2D : public Shape2D{
//     public:
//     Point2D p1() {return vertices[0];}
//     Point2D p2() {return vertices[1];}
//     Line2D(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
//         numberOfVerticies = 2;
//         shape = Line;
//         vertices = new Point2D[2];
//         vertices[0] = Point2D(x1, y1);
//         vertices[1] = Point2D(x2, y2);        
//     }
// };

// struct TriangleLeg{
//     int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
//     float slope = 0;
//     float yIntercept = 0;
//     bool isVertical = false;
//     bool isHorizontal = false;
//     TriangleLeg(){

//     }

//     TriangleLeg(int x1, int y1, int x2, int y2){
//         this->x1 = x1;
//         this->x2 = x2;
//         this->y1 = y1;
//         this->y2 = y2;
//         isVertical = x1 == x2;
//         isHorizontal = y1 == y2;
        
//         slope = isHorizontal ? 0 : isVertical ? 0 : ((float)(max(y1, y2) - min(y1, y2)) / (float)(y1 > y2? x1 - x2 : x2 - x1));
//         yIntercept = -1*(slope * x1 - y1);
//     }
// };

// struct TrinagleLegDrawObject : public TriangleLeg{
//     int rowX = 0, prevX  = 0;
//     TrinagleLegDrawObject(int x1, int y1, int x2, int y2): TriangleLeg(x1, y1, x2, y2){  
//         rowX = x1;
//         prevX = x2;
//     }
    
// };
// struct Triangle2D: public Shape2D{  
//     public:
//     Point2D p1() {return vertices[0];}
//     Point2D p2() {return vertices[1];}
//     Point2D p3() {return vertices[2];}
//     Triangle2D(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, FillStyle style = FillStyle::Outline){
//         numberOfVerticies = 3;
//         shape = Triangle;
//         this->style = style;
//         vertices = new Point2D[3]; 
//         vertices[0] = Point2D(x1, y1);
//         vertices[1] = Point2D(x2, y2);
//         vertices[2] = Point2D(x3, y3);
//     }
// };

struct Rectangle3D : public Shape3D{
    public:
    inline int16_t x1() {return vertices[0].x;};
    inline int16_t x2() {return vertices[1].x;};
    inline int16_t y1() {return vertices[0].y;};
    inline int16_t y2() {return vertices[1].y;};
    inline int16_t z1() {return vertices[0].z;};
    inline int16_t z2() {return vertices[1].z;};
    inline int16_t width() {return vertices[1].x - vertices[0].x;};
    inline int16_t height() {return vertices[1].y - vertices[0].y;};
    inline int16_t depth() {return vertices[1].z - vertices[0].z;};

    // int16_t width(){ return x2() > vertices[1].x ? vertices[1].x - vertices[0].x : vertices[0].x - vertices[1].x; } // abs(x2 - x1);}
    // int16_t height(){ return vertices[1].y > vertices[0].y ? vertices[1].y - vertices[0].y: vertices[0].y - vertices[1].y; } // abs(y2 - y1);}
    int32_t size() { return width() * height();}

    Rectangle3D(Point3D p1, Point3D p2, FillStyle style = FillStyle::Outline){
        numberOfVerticies = 4;
        shape = Rectangle;
        this->style = style;
        vertices = new Point3D[4];
        vertices[0] = Point3D(p1.x, p1.y, p1.z);
        vertices[1] = Point3D(p2.x, p1.y, p1.z);        
        vertices[2] = Point3D(p2.x, p2.y, p1.z);
        vertices[3] = Point3D(p1.x, p2.y, p1.z);
    }
    Rectangle3D(Point3D p1, int16_t width, int16_t height, int16_t depth = 0, FillStyle style = FillStyle::Outline){
        numberOfVerticies = 2;
        shape = Rectangle;
        this->style = style;
        vertices = new Point3D[4];
        vertices[0] = Point3D(p1.x, p1.y, p1.z);
        vertices[1] = Point3D(p1.x + width, p1.y, p1.z);        
        vertices[2] = Point3D(p1.x + width, p1.y + height, p1.z);
        vertices[3] = Point3D(p1.x,  p1.y + height, p1.z);             
    }
    Rectangle3D( int16_t x1, int16_t y1, int16_t z1, int16_t x2, int16_t y2, int16_t z2, FillStyle style = FillStyle::Outline){
        numberOfVerticies = 2;
        shape = Rectangle;
        this->style = style;
        //TODO: calculate Z for each verticie between defined
        vertices = new Point3D[4];
        vertices[0] = Point3D(x1, y1, z1);
        vertices[1] = Point3D(x2, y1, z1);        
        vertices[2] = Point3D(x2, y2, z1);
        vertices[3] = Point3D(x1, y2, z1);    
    }

    bool contains(int16_t x, int16_t y, int16_t z){
        return x >= vertices[0].x && x<= vertices[1].x &&
            y >= vertices[0].y && y<= vertices[1].y &&
            z >= vertices[0].z && z<= vertices[1].z;
    }
    bool contains(Point3D point){
        return contains(point.x, point.y, point.z);
    }
    bool intersects(Rectangle3D bounds){
        return  //if either shape contains a point from another
            contains(bounds.x1(), bounds.y1(), bounds.z1()) ||
            contains(bounds.x1(), bounds.y2(), bounds.z1()) ||
            contains(bounds.x2(), bounds.y1(), bounds.z1()) ||
            contains(bounds.x2(), bounds.y2(), bounds.z1()) ||
            contains(bounds.x1(), bounds.y1(), bounds.z2()) ||
            contains(bounds.x1(), bounds.y2(), bounds.z2()) ||
            contains(bounds.x2(), bounds.y1(), bounds.z2()) ||
            contains(bounds.x2(), bounds.y2(), bounds.z2()) ||
            bounds.contains(x1(), y1(), bounds.z1()) ||
            bounds.contains(x1(), y2(), bounds.z1()) ||
            bounds.contains(x2(), y1(), bounds.z1()) ||
            bounds.contains(x2(), y2(), bounds.z1()) ||
            bounds.contains(x1(), y1(), bounds.z2()) ||
            bounds.contains(x1(), y2(), bounds.z2()) ||
            bounds.contains(x2(), y1(), bounds.z2()) ||
            bounds.contains(x2(), y2(), bounds.z2());
    }
};

// struct Polygon2D : public Shape2D{  
//     public:
//     Polygon2D(Point2D* vertices, uint8_t size, FillStyle style = FillStyle::Outline){
//         this->numberOfVerticies = size;
//         shape = Polygon;
//         this->style = style;
//         this->vertices = new Point2D[size];
//         memcpy(this->vertices, vertices, size* sizeof(Point2D));
//     }
// };


// struct Texture2D{
//     public:
//     uint8_t width;
//     uint8_t height;
//     uint8_t *colors;
    
//     Texture2D(uint8_t width, uint8_t height)
//     {
//         this->width = width;
//         this->height = height;   
//         colors = new uint8_t[width * height];
//         memset(this->colors,Color::BLACK, width*height);    
//     }
//     Texture2D(uint8_t width, uint8_t height, uint8_t color)
//     {
//         this->width = width;
//         this->height = height;        
//         colors = new uint8_t[width * height];
//         memset(this->colors, color, width * height);
//     }  
//     Texture2D(uint8_t width, uint8_t height, uint8_t* data)
//     {
//         this->width = width;
//         this->height = height;        
//         colors = new uint8_t[width * height];
//         memcpy(this->colors,data, width*height); 
//     }  
//     //copy contructor
//     Texture2D(const Texture2D& o)
//         : width(o.width),
//           height(o.height)
//     {
//         colors = new uint8_t[width * height];
//         memcpy(this->colors,o.colors, width*height);
//         #ifdef DEBUG_GPU
//         Serial.println("Texture 2D deep copied");
//         #endif
//     }
//     // move constructor
//     Texture2D(Texture2D&& o) noexcept
//         : width(o.width),
//           height(o.height),
//           colors(o.colors)
//     {
//         o.colors = nullptr;
//         #ifdef DEBUG_GPU
//         Serial.println("Texture 2D moved");
//         #endif
//     }
    
//     void Fill(uint8_t color){
//         memset(this->colors,color, width*height);    
//     }   
//     ~Texture2D(){
//         delete[] colors;
//         colors = nullptr;
//     }
// };

#define Vector3D Point3D
#endif