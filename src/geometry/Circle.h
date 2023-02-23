#pragma once

#include "Point.h"

class Circle
{
    public:
        Circle(Point center, float radius) { m_Center = center; m_Radius = radius; }
        virtual ~Circle() {}	

        Point GetCenter() const { return m_Center; }
        float GetRadius() const { return m_Radius; }

    private:
        Point m_Center;
        float m_Radius;
};

