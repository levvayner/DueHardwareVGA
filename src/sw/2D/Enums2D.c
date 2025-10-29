#include "Enums2D.h"
const char * ShapeName[] = {
    "Pixel",
    "Circle",
    "Oval",
    "Arc",
    "Line",
    "Triangle",
    "Rectangle",
    "Polygon"
};
const char * GetShapeName(int shape){
    return ShapeName[shape];
}