#pragma once

#include "Point.h"


//! \class Triangle
class Triangle
{
    public:
        Triangle() { m_P1 = Point(); m_P2 = Point(); m_P3 = Point(); }
        Triangle(const Point& p1, const Point& p2, const Point& p3) { m_P1 = p1; m_P2 = p2; m_P3 = p3;}
        virtual ~Triangle() {}

        const Point& GetP1() const { return m_P1;}
        const Point& GetP2() const { return m_P2;}
        const Point& GetP3() const { return m_P3;}

        void SetP1(const Point& p1) { m_P1 = p1;}
        void SetP2(const Point& p2) { m_P2 = p2;}
        void SetP3(const Point& p3) { m_P3 = p3;}


    private:
        Point m_P1;
        Point m_P2;
        Point m_P3;
};