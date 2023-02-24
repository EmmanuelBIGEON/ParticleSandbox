#pragma once

#include "../../main/GraphicObject.h"
#include "../../main/GraphicContext.h"

#include "../../../geometry/Line.h"

#include <glm/glm.hpp>

#include "../../main/Shader.h"

class GraphicLine : public GraphicObject
{
    public:
        GraphicLine(GraphicContext* context, const Line& line, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
        virtual ~GraphicLine();

        void SetColor(const glm::vec3& color) { m_Color = color; m_IsUpdated = false; }

        virtual void Update();
        virtual void Draw();
    private:
        Line m_Line;
        glm::vec3 m_Color;
        glm::mat2 m_rotationMatrixInversed;
        Shader* m_Shader;

        unsigned int m_VAO;
        unsigned int m_VBO;
        float m_vertices[4];
};
