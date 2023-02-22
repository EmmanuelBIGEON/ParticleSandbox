#pragma once

#include "../../geometry/Triangle.h"
#include "../main/GraphicObject.h"
#include "../main/GraphicContext.h"

#include <glm/vec3.hpp>

//! \class TriangleAdapter
//! \brief A class that adapts a triangle to the graphics context for it to be displayed.
class TriangleAdapter : public GraphicObject
{
    public: 
        TriangleAdapter(GraphicContext* context, const Triangle& triangle);
        virtual ~TriangleAdapter();

        const Triangle& GetTriangle() const { return m_Triangle;}
        void SetTriangle(const Triangle& triangle) { m_Triangle = triangle;}

        void SetColor(const glm::vec3& color);
        void SetOpacity(float alpha);

        virtual void Update() override;
        virtual void Draw() override;

    private:
        Triangle m_Triangle;

        // opengl
        unsigned int m_VAO; // Might be possible to optimize by using only one VAO for all triangles and basic (SHADER_BASIC)
        unsigned int m_VBO;

        glm::vec3 m_Color;
        float m_Alpha;

        float m_vertices[21];

};