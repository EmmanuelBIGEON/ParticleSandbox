#include "ParticleGenerator.h"

#include <iostream>

ParticleGenerator::ParticleGenerator(GraphicContext* context, const glm::vec2& bottomLeft, const glm::vec2& topRight)
    : m_Context(context), m_BottomLeft(bottomLeft), m_TopRight(topRight)
{
}

ParticleGenerator::~ParticleGenerator()
{
}

void ParticleGenerator::Generate(int nbParticles)
{
    for (int i = 0; i < nbParticles; i++)
    {
        float x = m_BottomLeft.x + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(m_TopRight.x - m_BottomLeft.x)));
        float y = m_BottomLeft.y + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(m_TopRight.y - m_BottomLeft.y)));
        glm::vec2 position = glm::vec2(x, y);
        Particle* particle = new Particle(position);
        ParticleAdapter* particleAdapter = new ParticleAdapter(m_Context, particle);
    }
}
