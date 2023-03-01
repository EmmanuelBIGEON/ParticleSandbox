#pragma once

#include "../../components/adapters/ParticleAdapter.h"
#include "../../main/GraphicContext.h"

#include <glm/glm.hpp>

class ParticleGenerator
{
    public:
        ParticleGenerator(GraphicContext* context, const glm::vec2& bottomLeft, const glm::vec2& topRight);
        virtual ~ParticleGenerator();

        void Generate(int nbParticles, ParticleClass particleClass = ParticleClass::PARTICLE_CLASS_A);
    
    private:
        GraphicContext* m_Context;
        glm::vec2 m_BottomLeft;
        glm::vec2 m_TopRight;
};