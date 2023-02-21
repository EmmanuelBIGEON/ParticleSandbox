#pragma once


class Point;
class Point3D;

//! \class Point
//! \brief A basic 2D point.
class Point
{
    public: 
        Point(double x, double y) { this->x = x; this->y = y; }
        Point() { x = 0; y = 0; }
        Point(const Point& point) { x = point.x; y = point.y; }
        Point(const Point3D& point) { x = point.x; y = point.y; }
        virtual ~Point() {}
    
        double x;
        double y;

};

//! \class Point3D
//! \brief A basic 3D point.
class Point3D
{
    public:
        Point3D(double x, double y, double z) { this->x = x; this->y = y; this->z = z; }
        Point3D() { x = 0; y = 0; z = 0; }
        Point3D(const Point3D& point) { x = point.x; y = point.y; z = point.z; }
        Point3D(const Point& point) { x = point.x; y = point.y; z = 0; }
        virtual ~Point3D() {}

        double x;
        double y;
        double z;
};