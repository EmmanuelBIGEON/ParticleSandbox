#include "Particle.h"

#include <glm/glm.hpp>

Particle::Particle(): Object(), m_Position(0.0f), m_Velocity(0.0f), m_Force(1.0f), m_Mass(0.0f), m_Radius(0.0f)
{
}

// Particle::Particle(const glm::vec2& position, const glm::vec2& velocity, const glm::vec2& force, float mass, float radius)
//     : Object(), m_Position(position), m_Velocity(velocity), m_Force(force), m_Mass(mass), m_Radius(radius)
// {
// }

// Particle::Particle(const glm::vec2& position, float radius) 
//     : Object(), m_Position(position), m_Velocity(0.0f), m_Force(0.0f), m_Mass(0.0f), m_Radius(radius)
// {
// }

Particle::Particle(const glm::vec2& position)
    : Object(), m_Position(position), m_Velocity(0.0f), m_Force(2.0f), m_Mass(0.0f), m_Radius(0.0f)
{
}

Particle::~Particle()
{
}

bool Particle::CheckCollision(const Particle& particle) const 
{
    float distance = glm::distance(m_Position, particle.m_Position);
    return distance < m_Radius + particle.m_Radius;
}
