#include "Graphics2DObject.h"
#include <ShapeList.h>
class Graphics2D{
    public: 
    //GraphicsObject2D* currentDrawObject = nullptr;
    ShapeList<GraphicsObject2D>* shapeList = new ShapeList<GraphicsObject2D>(); 
    ~Graphics2D(){
        if(shapeList){
            // clear elements so their destructors free owned shape/texture
            shapeList->clear();
            delete shapeList;
            shapeList = nullptr;
        }
    }
};