#pragma once

#include "Point.h"

//! \class Rectangle
class Rectangle
{
    public:
        Rectangle() { m_P1 = Point(); m_P2 = Point();}
        Rectangle(const Point& p1, const Point& p2) { m_P1 = p1; m_P2 = p2;}
        virtual ~Rectangle() {}

        const Point& GetP1() const { return m_P1;}
        const Point& GetP2() const { return m_P2;}

        void SetP1(const Point& p1) { m_P1 = p1;}
        void SetP2(const Point& p2) { m_P2 = p2;}

    private:
        Point m_P1;
        Point m_P2;
};