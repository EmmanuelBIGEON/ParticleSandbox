#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"
#include "../../main/Shader.h"

#include "../../../geometry/Circle.h"

#include <glm/glm.hpp>

class WigglingCircle : public GraphicObject
{
    public:
        WigglingCircle(GraphicContext* context, const Circle& circle);
        virtual ~WigglingCircle();

        const Circle& GetCircle() const { return m_Circle; }
        void SetCircle(const Circle& circle) { m_Circle = circle; m_IsUpdated=false; needRecompute=true;}
        void SetColor(const glm::vec3& color) { m_Color = color; m_IsUpdated=false; needRecompute=true;}
        
        virtual void Update() override;
        virtual void Draw() override;

    private:
        Shader* m_Shader;
        Circle m_Circle;
        glm::vec3 m_Color;

        glm::mat4 m_shiftPos;
        bool m_goingLeft;
        float m_shift;

        float* m_vertices;
        int m_nbVertices;

        unsigned int m_VAO;
        unsigned int m_VBO;
        
        bool needRecompute;

};