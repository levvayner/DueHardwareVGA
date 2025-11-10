#include "Graphics2DObject.h"
#include <ShapeList.h>
class Graphics2D{
    public: 
    //Graphics2DObject* currentDrawObject = nullptr;
    ShapeList<Graphics2DObject>* shapeList = nullptr; 
    Graphics2D()
    : shapeList(new ShapeList<Graphics2DObject>()) {}

    ~Graphics2D() {
        shapeList->clear();
        delete shapeList;
        shapeList = nullptr;
    }

    // ~Graphics2D(){
    //     if(shapeList){
    //         // clear elements so their destructors free owned shape/texture
    
    //         delete shapeList;
    //         shapeList = nullptr;
    //     }
    // }
};