#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include "../../../geometry/Circle.h"

#include <glm/glm.hpp>

class PanickedCircle : public GraphicObject
{
    public:
        PanickedCircle(GraphicContext* context, const Circle& circle);
        virtual ~PanickedCircle();

        const Circle& GetCircle() const { return m_Circle; }
        void SetCircle(const Circle& circle) { m_Circle = circle; m_IsUpdated=false;}
        void SetColor(const glm::vec3& color) { m_Color = color; m_IsUpdated=false;}
        
        virtual void Update() override;
        virtual void Draw() override;

    private:

        Circle m_Circle;
        glm::vec3 m_Color;

        float* m_vertices;
        int m_nbVertices;

        unsigned int m_VAO;
        unsigned int m_VBO;
        
        bool firstTimeCalled;

};