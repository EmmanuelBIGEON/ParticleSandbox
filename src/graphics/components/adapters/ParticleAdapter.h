#pragma once

#include "../../../core/Particle.h"

#include <glm/glm.hpp>
#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"
#include "../../main/Shader.h"


//! \class ParticleAdapter
//! \brief Adapter for Particle class.
class ParticleAdapter : public GraphicObject
{
    public:
        ParticleAdapter(GraphicContext* context, Particle* particle);
        virtual ~ParticleAdapter();

        Particle* GetParticle() { return m_Particle; }

        void SetColor(const glm::vec3& color) { m_Color = color; }

        virtual void Update() override;
        virtual void Draw() override;

        // Highligth the particle.
        void SetHighlight(bool highlight){ m_Highlight = highlight;}

        //! \brief Initialize the VAO and VBO for the particle.
        //! The purpose is to reduce as much as possible the computation needed to draw the particle.
        static void LoadParticleVAO();

        glm::lowp_vec2 m_Position;
        static glm::vec3 highlightColor;
        static unsigned int VAO;
    private:
        static unsigned int VBO;
        static float* vertices;
        static int nbVertices;

        float mvt_x;
        float mvt_y;
        
        glm::vec3 m_Color;
        bool m_Highlight;

        Shader* m_Shader;
        Particle* m_Particle;

};
