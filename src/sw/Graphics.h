// #ifndef _GRAPHICS_H
// #define _GRAPHICS_H
// #include "hw/video/VRAM.h"
// #include "sw/3D/Objects3D.h"
// class Graphics{
//     public:
//     static inline int Distance(Point2D point1, Point2D point2){
//         int difX = (point2.x - point1.x);
//         int difY = (point2.y - point1.y);

//         return sqrt( (difX*difX) + (difY*difY));
//     }

//     static inline int Distance(Point3D point1, Point3D point2){
//         int difX = (point2.x - point1.x);
//         int difY = (point2.y - point1.y);
//         int difZ = (point2.z - point1.z);

//         return sqrt( (difX*difX) + (difY*difY) + (difZ*difZ));
//     }
//     static inline Vector2D DistanceVector(Point2D point1, Point2D point2){  

//         return Point2D((point2.x - point1.x), (point2.y - point1.y));
//     }
//     static inline Vector3D DistanceVector(Point3D point1, Point3D point2){  

//         return Point3D((point2.x - point1.x), (point2.y - point1.y), (point2.z - point1.z));
//     }

    
//     /// @brief Projects a 3d point on to a 2d plane
//     /// @param point 3d point to project
//     /// @param b 0 for x, 1 for y
//     /// @return projected value of axis requested by b
//     static inline double Project(Point3D point, int b){
//         int fovl=200;
//         double oldpos= b == 0 ? point.x : b == 1 ? point.y : point.z;
//         double z=point.z;
//         double newpos=(double)(fovl/(fovl+z))*oldpos;
//         //want to move it off screen as we get closer, z is smaller
        
//         return newpos;
//     }
//     static inline Point3D Project3D(Point3D camera, Point3D object){
//         auto distanceToCamera = DistanceVector(camera,object);
        
//         return Point3D(Project(distanceToCamera,0), Project(distanceToCamera,1), distanceToCamera.z );
//     }
//     static inline Point3D Project3D(Point3D camera, Vector3D cameraOrientation, Point3D object){
//         auto distanceToCamera = DistanceVector(camera,object);
//         auto invCamera = Vector3D(cameraOrientation.x * -1, cameraOrientation.y * -1, cameraOrientation.z * -1);
//         auto distanceRotated = DistanceVector(distanceToCamera,invCamera);
//         auto projected = Point3D(Project(distanceRotated,0), Project(distanceRotated,1), distanceRotated.z );
       
//         return projected;
//     }
// };
// #endif