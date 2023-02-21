#pragma once

#include "../../geometry/Triangle.h"
#include "../main/GraphicObject.h"
#include "../main/GraphicContext.h"

//! \class TriangleAdapter
//! \brief A class that adapts a triangle to the graphics context for it to be displayed.
class TriangleAdapter : public GraphicObject
{
    public: 
        TriangleAdapter(const Triangle& triangle);
        virtual ~TriangleAdapter();

        const Triangle& GetTriangle() const;
        void SetTriangle(const Triangle& triangle);

    private:
        Triangle m_Triangle;
};