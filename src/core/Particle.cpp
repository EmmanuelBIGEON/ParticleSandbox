#include "Particle.h"

#include <glm/glm.hpp>

float Particle::Radius = 9.0f;

Particle::Particle(ParticleClass particleClass): Object(), m_Position(0.0f), m_Mass(1.0f), m_ParticleClass(particleClass)
{
}

Particle::Particle(const glm::vec2& position, ParticleClass particleClass)
    : Object(), m_Position(position), m_Mass(1.0f), m_ParticleClass(particleClass)
{
}

Particle::~Particle()
{
}

