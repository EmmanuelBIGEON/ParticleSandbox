#pragma once

#include "../../main/GraphicObject.h"
#include "../../main/GraphicContext.h"

#include "../../../geometry/Line.h"

#include <glm/glm.hpp>

#include "../../main/Shader.h"


class BezierCurve : public GraphicObject
{
    public:
        BezierCurve(GraphicContext* context, const Line& line);
        virtual ~BezierCurve();

        void SetColor(const glm::vec3& color);
        void SetLine(const Line& line);

        const Line& GetLine() const { return m_Line; }
        const glm::vec3& GetColor() const { return m_Color; }

        // controls points
        void SetControlPoint1(const glm::vec2& point) { m_ControlPoint1 = point; m_IsUpdated = false;}
        void SetControlPoint2(const glm::vec2& point) { m_ControlPoint2 = point; m_IsUpdated = false;}


        virtual void Update() override;
        virtual void Draw() override;

    private:

        Line m_Line;
        Point m_ControlPoint1;
        Point m_ControlPoint2;
        glm::mat3 m_rotationMatrixInversed;
        Shader* m_Shader;

        glm::vec3 m_Color;
        
        unsigned int m_VAO;
        unsigned int m_VBO;

        float* m_vertices;
};