#pragma once

#include "Point.h"

//! \class Rectangle
class Rectangle
{
    public:
        Rectangle();
        Rectangle(const Point& p1, const Point& p2);
        virtual ~Rectangle();

        const Point& GetP1() const;
        const Point& GetP2() const;

        void SetP1(const Point& p1);
        void SetP2(const Point& p2);

    private:
        Point m_P1;
        Point m_P2;
};