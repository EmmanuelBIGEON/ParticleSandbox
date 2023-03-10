#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"
#include "../../main/Shader.h"

#include "../../../geometry/Rectangle.h"

#include <glm/glm.hpp>

class RectangleAdapter : public GraphicObject
{
    public:
        RectangleAdapter(GraphicContext* context, const Rectangle& rectangle);
        virtual ~RectangleAdapter();

        const Rectangle& GetRectangle() const { return m_Rectangle;}
        void SetRectangle(const Rectangle& rectangle) { m_Rectangle = rectangle;}

        void SetColor(const glm::vec3& color);
        void SetOpacity(float alpha);

        virtual void Update() override;
        virtual void Draw() override;

    private:
        Shader* m_Shader;
        Rectangle m_Rectangle;

        unsigned int m_VAO; // Might be possible to optimize 
        unsigned int m_VBO;
        unsigned int m_EBO;

        glm::vec3 m_Color;
        float m_Alpha;

        float m_vertices[28];

};