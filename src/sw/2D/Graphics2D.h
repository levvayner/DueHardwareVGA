#include "Arduino.h"
#include "sw/Color.h"
enum FillStyle{
    Outline = 0,
    Fill = 1
};
enum Shape{
    Pixel = 1,
    Circle = 2,
    Oval = 3,
    Arc = 4,
    Line = 5,
    Triangle = 6,
    Rectangle = 7,
    Polygon = 8
};

struct Point2D{
    int16_t x;
    int16_t y;
    Point2D(): x(0), y(0){};
    Point2D(int16_t x, int16_t y){ this->x = x; this->y = y;}

    bool operator ==(Point2D& other){
        return x == other.x && y == other.y;
    }
    bool operator !=(Point2D& other){
        return x != other.x || y != other.y;
    }
};

class Shape2D {
    public:
    uint8_t numberOfVerticies;
    Point2D* vertecies;
    FillStyle style = FillStyle::Outline; // e.g. 0 is outline, 1 is fill, 2 is vertical gradient...
    //TODO: dynamically find shape like dynamic_cast
    Shape shape;
    virtual ~Shape2D() = default; //virtual destructor makes class abstract

    virtual bool operator==(Shape2D& other){
        return numberOfVerticies == other.numberOfVerticies &&
        style == other.style ;
        // TODO: iterate and compare
    }
};

class Circle2D : public Shape2D{
    public:
    uint16_t radius;
    Circle2D(int16_t x, int16_t y, uint16_t radius){
        numberOfVerticies = 1;
        shape = Circle;
        vertecies = new Point2D[1]; 
        vertecies[0] = Point2D(x, y);
        //vertecies.push_back(Point2D(x, y));
        this->radius = radius;
    }
};

class Oval2D : public Shape2D{
    public:
    uint16_t radiusX;
    uint16_t radiusY;
    Oval2D(int16_t x, int16_t y, uint16_t radiusX, uint16_t radiuxY){
        numberOfVerticies = 1;
        shape = Oval;
        vertecies = new Point2D[1]; 
        vertecies[0] = Point2D(x, y);
        this->radiusX = radiusX;
        this->radiusY = radiusY;
    }
};

class Arc2D : public Shape2D{
    public:
    uint16_t radius;
    uint8_t startDeg;
    uint8_t endDeg;
    Arc2D(int16_t x, int16_t y, uint16_t radius, uint8_t startDeg, uint8_t endDeg){
        this->numberOfVerticies = 1;
        shape = Arc;
        vertecies = new Point2D[2]; 
        vertecies[0] = Point2D(x, y);
        this->radius = radius;
        this->startDeg = startDeg;
        this->endDeg = endDeg; 
    }
};


class Line2D : public Shape2D{
    public:
    Point2D p1() {return vertecies[0];}
    Point2D p2() {return vertecies[1];}
    Line2D(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
        numberOfVerticies = 2;
        shape = Line;
        vertecies = new Point2D[2];
        vertecies[0] = Point2D(x1, y1);
        vertecies[1] = Point2D(x2, y2);        
    }
};

struct TriangleLeg{
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    float slope = 0;
    float yIntercept = 0;
    bool isVertical = false;
    bool isHorizontal = false;
    TriangleLeg(){

    }

    TriangleLeg(int x1, int y1, int x2, int y2){
        this->x1 = x1;
        this->x2 = x2;
        this->y1 = y1;
        this->y2 = y2;
        isVertical = x1 == x2;
        isHorizontal = y1 == y2;
        
        slope = isHorizontal ? 0 : isVertical ? 0 : ((float)(max(y1, y2) - min(y1, y2)) / (float)(y1 > y2? x1 - x2 : x2 - x1));
        yIntercept = -1*(slope * x1 - y1);
    }
};

struct TrinagleLegDrawObject : public TriangleLeg{
    int rowX = 0, prevX  = 0;
    TrinagleLegDrawObject(int x1, int y1, int x2, int y2): TriangleLeg(x1, y1, x2, y2){  
        rowX = x1;
        prevX = x2;
    }
    
};
class Triangle2D: public Shape2D{  
    public:
    Point2D p1() {return vertecies[0];}
    Point2D p2() {return vertecies[1];}
    Point2D p3() {return vertecies[2];}
    Triangle2D(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3){
        numberOfVerticies = 3;
        shape = Triangle;
        vertecies = new Point2D[3]; 
        vertecies[0] = Point2D(x1, y1);
        vertecies[1] = Point2D(x2, y2);
        vertecies[2] = Point2D(x3, y3);
    }
};

class Rectangle2D : public Shape2D{
    public:
    int16_t x1() {return vertecies[0].x;};
    int16_t x2() {return vertecies[1].x;};
    int16_t y1() {return vertecies[0].y;};
    int16_t y2() {return vertecies[1].y;};

    int16_t width(){ return x2() > vertecies[1].x ? vertecies[1].x - vertecies[0].x : vertecies[0].x - vertecies[1].x; } // abs(x2 - x1);}
    int16_t height(){ return vertecies[1].y > vertecies[0].y ? vertecies[1].y - vertecies[0].y: vertecies[0].y - vertecies[1].y; } // abs(y2 - y1);}
    int32_t size() { return width() * height();}

    Rectangle2D(Point2D p1, Point2D p2){
        numberOfVerticies = 2;
        shape = Rectangle;
        vertecies = new Point2D[2];
        vertecies[0] = p1;
        vertecies[1] = p2;        
    }
    Rectangle2D( int16_t x1, int16_t y1, int16_t x2, int16_t y2){
        numberOfVerticies = 2;
        shape = Rectangle;
        vertecies = new Point2D[2];
        vertecies[0] = Point2D(x1,y1);
        vertecies[1] = Point2D(x2,y2);                
    }
};

class Polygon2D : public Shape2D{  
    public:
    Polygon2D(Point2D* vertecies, uint8_t size){
        this->numberOfVerticies = size;
        shape = Polygon;
        vertecies = new Point2D[size];
        memcpy(this->vertecies, vertecies, size);
    }
};


class Texture2D{
    public:
    uint8_t width;
    uint8_t height;
    uint8_t colors[64];
    
    Texture2D(uint8_t width, uint8_t height)
    {
        this->width = width;
        this->height = height;   
        memset(this->colors,Color::BLACK, width*height);    
    }
    Texture2D(uint8_t width, uint8_t height, uint8_t* data)
    {
        this->width = width;
        this->height = height;        
        memcpy(this->colors,data, width*height); 
    }   
};

struct GraphicsObject2D{

    Shape2D* shape;
    Texture2D* texture;
    bool drawnOnMem1 = false;
    bool drawnOnMem2 = false;

    GraphicsObject2D(Shape2D* shape, Texture2D* texture){
        this->shape = shape;
        this->texture = texture;
    }
};

class Graphics2D{
    public: 
    GraphicsObject2D*  objects;
    int objectCount;
};