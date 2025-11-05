
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

    Point3D& operator=(const Point2D& other) {
        if (this->x == other.x && this->y == other.y) return *this;
        this->x = other.x;
        this->y = other.y;        
        return *this;
    }

    Point3D& operator=(Point2D&& o) noexcept {
        if (this->x == o.x && this->y == o.y) return *this;
        this->x = o.x;
        this->y = o.y;        
        return *this;
    }

    bool operator==(const Point2D& p2d) const { return p2d.x == x && p2d.y == y; }

};

#define Vector3D Point3D