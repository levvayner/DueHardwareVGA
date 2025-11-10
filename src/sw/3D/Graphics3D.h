#ifndef GRAPHICS_3D_H
#define GRAPHICS_3D_H
#include "Graphics3DObject.h"
#include <ShapeList.h>
class Graphics3D{
    public: 
    ShapeList<Graphics3DObject>* shapeList = nullptr; 
    
    Graphics3D()
    : shapeList(new ShapeList<Graphics3DObject>()) {}

    ~Graphics3D() {
        shapeList->clear();
        delete shapeList;
        shapeList = nullptr;
    }

    inline Point3D GetCamera(){ return _camera;}
    inline void UpdateCamera(Point3D newPosition){        
        if( shapeList == nullptr){
            Serial.println("Objects not instantiated.");
            return;
        }
        //graphics.clear();
        Serial.print("Found ");Serial.print(shapeList->size());Serial.println(" objects");
        // for(uint16_t idx=0;idx<shapeList->size();idx++){
        //     Graphics3DObject * obj = &shapeList->get(idx);
        //     Graphics3D::UpdateObjectProjection(newPosition, _cameraOrientation, obj);
        //     obj->projected->setFillStyle( obj->projected->area() < 3000 ? Fill : Outline);
        // }
        for(auto &cosmicObject : *shapeList){
            Graphics3D::UpdateObjectProjection(newPosition, _cameraOrientation, &cosmicObject);
            //cosmicObject.projected->setFillStyle( cosmicObject.projected->area() < 3000 ? Fill : Outline);
        } 
    }

    static inline int Distance(Point2D point1, Point2D point2){
        int difX = (point2.x - point1.x);
        int difY = (point2.y - point1.y);

        return sqrt( (difX*difX) + (difY*difY));
    }

    static inline int Distance(Point3D point1, Point3D point2){
        int difX = (point2.x - point1.x);
        int difY = (point2.y - point1.y);
        int difZ = (point2.z - point1.z);

        return sqrt( (difX*difX) + (difY*difY) + (difZ*difZ));
    }
    static inline Vector2D DistanceVector(Point2D point1, Point2D point2){  

        return Point2D((point2.x - point1.x), (point2.y - point1.y));
    }
    static inline Vector3D DistanceVector(Point3D point1, Point3D point2){  

        return Point3D((point2.x - point1.x), (point2.y - point1.y), (point2.z - point1.z));
    }

    
    

    

    private:

    Point3D _camera = Point3D(0,0,0);
    Point3D _cameraOrientation = Point3D(0,0,0);

    /// @brief Projects a 3d point on to a 2d plane
    /// @param point 3d point to project
    /// @param b 0 for x, 1 for y
    /// @return projected value of axis requested by b
    static inline double Project(Point3D point, int b){
        int fovl=200;
        double oldpos= b == 0 ? point.x : b == 1 ? point.y : point.z;
        double z=point.z;
        double newpos=(double)(fovl/(fovl+z))*oldpos;
        //want to move it off screen as we get closer, z is smaller
        
        return newpos;
    }
    static inline Point3D Project3D(Point3D camera, Point3D object){
        auto distanceToCamera = DistanceVector(camera,object);
        
        return Point3D(Project(distanceToCamera,0), Project(distanceToCamera,1), distanceToCamera.z );
    }
    static inline Point3D Project3D(Point3D camera, Vector3D cameraOrientation, Point3D object){
        // auto distanceToCamera = DistanceVector(camera,object);
        // auto invCamera = Vector3D(cameraOrientation.x * -1, cameraOrientation.y * -1, cameraOrientation.z * -1);
        // auto distanceRotated = DistanceVector(distanceToCamera,invCamera);
        // auto projected = Point3D(Project(distanceRotated,0), Project(distanceRotated,1), distanceRotated.z );
       
        // return projected;
        return object;
    }

    static inline void UpdateObjectProjection(Point3D camera, Point3D cameraOrientation, Graphics3DObject * obj){
        char buf[256] = {0};
        if( obj->projected == nullptr){
            obj->projected = new Rectangle2D(obj->shape->vertices[0],obj->shape->vertices[2]);
        }
        for(int vertIdx = 0; vertIdx < obj->shape->numberOfVerticies; vertIdx++){
            obj->projected->vertices[vertIdx] = Project3D(camera,cameraOrientation, obj->shape->vertices[vertIdx]);
        }
        //TODO: add inverted flag to swap or not
        //swap verities
        // auto vtemp = obj->projected->vertices[0];
        // obj->projected->vertices[0] = obj->projected->vertices[1];
        // obj->projected->vertices[1] = vtemp;
        // vtemp = obj->projected->vertices[2];
        // obj->projected->vertices[2] = obj->projected->vertices[3];
        // obj->projected->vertices[3] = vtemp;


        auto area = obj->projected->area();
        sprintf(buf,"%d,%d \n %0.0f", obj->projected->vertices[0].x, obj->projected->vertices[0].y, area);
        obj->update_text(buf);
        //(128.0f/(float)(obj->shape->vertices[0].z + 10));        
        #if defined(DEBUG_GPU) && DEBUG_GPU > 0
        
        sprintf(buf,"[3D Projection] Projected shape at (%d,%d,%d), (%d,%d,%d), (%d,%d,%d), (%d,%d,%d) to (%d,%d), (%d,%d), (%d,%d), (%d,%d)",
            obj->shape->vertices[0].x, obj->shape->vertices[0].y, obj->shape->vertices[0].z,
            obj->shape->vertices[1].x, obj->shape->vertices[1].y, obj->shape->vertices[1].z,
            obj->shape->vertices[2].x, obj->shape->vertices[2].y, obj->shape->vertices[2].z,
            obj->shape->vertices[3].x, obj->shape->vertices[3].y, obj->shape->vertices[3].z,
            obj->projected->vertices[0].x, obj->projected->vertices[0].y,
            obj->projected->vertices[1].x, obj->projected->vertices[1].y,
            obj->projected->vertices[2].x, obj->projected->vertices[2].y,
            obj->projected->vertices[3].x, obj->projected->vertices[3].y
        );
        Serial.println(buf);
        #endif
        obj->invalidate();
        
    }

    // ~Graphics3D(){
    //     if(shapeList){
    //         // clear elements so their destructors free owned shape/texture
    
    //         delete shapeList;
    //         shapeList = nullptr;
    //     }
    // }
};
#endif