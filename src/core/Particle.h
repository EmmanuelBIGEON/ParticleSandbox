#pragma once

#include "../common/Object.h"

#include <glm/glm.hpp>

//! \class Particle
//! \brief Main object of the simulation.
//! It is a point with different properties.
class Particle : public Object
{
    public:
        Particle();
        // Particle(const glm::vec2& position, const glm::vec2& velocity, const glm::vec2& force, float mass, float radius);
        // Particle(const glm::vec2& position, float radius); 
        Particle(const glm::vec2& position);

        virtual ~Particle();

        //! \brief Get the position of the particle.
        const glm::vec2& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec2& position) { m_Position = position; }

        //! \brief Get the velocity of the particle.
        const glm::vec2& GetVelocity() const { return m_Velocity; }
        void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }

        //! \brief Get the current force of the particle.
        const glm::vec2& GetForce() const { return m_Force; }
        void SetForce(const glm::vec2& force) { m_Force = force; }

        //! \brief Get the mass of the particle.
        float GetMass() const { return m_Mass; }
        void SetMass(float mass) { m_Mass = mass; }

        //! \brief Get the radius of the particle.
        float GetRadius() const { return m_Radius; }
        void SetRadius(float radius) { m_Radius = radius; }

        //! \brief Check for collisions with the given particle.
        bool CheckCollision(const Particle& particle) const;

    private:
        glm::vec2 m_Position; //! The position of the particle.
        glm::vec2 m_Velocity; //! The velocity of the particle.
        glm::vec2 m_Force; //! The current force of the particle.
        float m_Mass; //! The mass of the particle.
        float m_Radius; //! The radius of the particle.
};