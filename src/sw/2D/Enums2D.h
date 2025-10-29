#ifndef ENUMS2D_H
#define ENUMS2D_H
enum FillStyle{
    Outline = 0,
    Fill = 1
};
enum Shape{
    Pixel = 0,
    Circle = 1,
    Oval = 2,
    Arc = 3,
    Line = 4,
    Triangle = 5,
    Rectangle = 6,
    Polygon = 7
};
extern const char * ShapeName[];
extern const char * GetShapeName(int shape);
#endif