#pragma once

#include <glm/ext.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Point;
class Point3D;

//! \class Point3D
//! \brief A basic 3D point.
class Point3D
{
    public:
        Point3D(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
        Point3D() { x = 0; y = 0; z = 0; }
        Point3D(const Point3D& point) { x = point.x; y = point.y; z = point.z; }
        Point3D(const glm::vec3& point) { x = point.x; y = point.y; z = point.z; }
        virtual ~Point3D() {}

        glm::vec3 ToVec3() const { return glm::vec3(x, y, z); }

        // operator []
        float& operator[](int index) { return (&x)[index]; }


        float x;
        float y;
        float z;
};

//! \class Point
//! \brief A basic 2D point.
class Point
{
    public: 
        Point(float x, float y) { this->x = x; this->y = y; }
        Point() { x = 0; y = 0; }
        Point(const Point& point) { x = point.x; y = point.y; }
        Point(const Point3D& point) { x = point.x; y = point.y; }
        Point(const glm::vec2& point) { x = point.x; y = point.y; }
        virtual ~Point() {}
        
        glm::vec3 ToVec3() const { return glm::vec3(x, y, 1.0f); }
        glm::vec2 ToVec2() const { return glm::vec2(x, y); }
    
        float x;
        float y;

};
