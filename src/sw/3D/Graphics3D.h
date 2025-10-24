
struct Point3D{
    int x;
    int y;
    int z;
    Point3D(){
        x = 0;
        y = 0;
        z = 0;
    }
    Point3D(int x, int y, int z){ this->x = x; this->y = y; this->z = z;}
};

#define Vector3D Point3D