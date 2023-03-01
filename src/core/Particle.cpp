#include "Particle.h"

#include <glm/glm.hpp>

float Particle::Radius = 13.0f;

Particle::Particle(): Object(), m_Position(0.0f), m_Mass(1.0f)
{
}

Particle::Particle(const glm::vec2& position)
    : Object(), m_Position(position), m_Mass(1.0f)
{
}

Particle::~Particle()
{
}