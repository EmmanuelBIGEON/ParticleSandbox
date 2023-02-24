#pragma once

#include "Point.h"

#include <vector>

#include <glm/vec2.hpp>

class Line
{
    public:
        Line(float x1, float y1, float x2, float y2) { start = Point(x1, y1); end = Point(x2, y2); }
        Line(Point pstart, Point pend) { start = pstart; end = pend; }
        Line(glm::vec2 vstart, glm::vec2 vend) { start = Point(vstart.x, vstart.y); end = Point(vend.x, vend.y); }
        Line() { start = Point(0, 0); end = Point(0, 0); }

        virtual ~Line() {}

        Point start;
        Point end;

    private:
};
