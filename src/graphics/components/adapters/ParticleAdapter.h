#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include <glm/ext.hpp>

class Shader;

//! \class ParticleAdapter
//! \brief This class is used to draw a particle on screen without interaction.
class ParticleAdapter : public GraphicObject
{
    public:
        ParticleAdapter(GraphicContext* context, const glm::vec2& position, const glm::vec3& color, float size = 25.0f);
        ~ParticleAdapter();

        virtual void Update() override;
        virtual void Draw() override;

        void SetColor(const glm::vec3& color) { m_Color = color;}
        void SetSize(float size) { m_Size = size; m_IsUpdated = false;}
    
        static unsigned int VAO;
        static unsigned int VBO;
        static float* vertices;
        static int nbVertices;
    private:

        Shader* m_Shader;
        glm::vec2 m_Position;
        glm::vec3 m_Color;
        float m_Size;

};