#pragma once

#include "Point.h"

//! \class Triangle
class Triangle
{
    public:
        Triangle();
        Triangle(const Point& p1, const Point& p2, const Point& p3);
        virtual ~Triangle();

        const Point& GetP1() const;
        const Point& GetP2() const;
        const Point& GetP3() const;

        void SetP1(const Point& p1);
        void SetP2(const Point& p2);
        void SetP3(const Point& p3);

    private:
        Point m_P1;
        Point m_P2;
        Point m_P3;
};