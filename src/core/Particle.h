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
        Particle(const glm::vec2& position);

        virtual ~Particle();

        //! \brief Get the position of the particle.
        glm::lowp_vec2& GetPosition() { return m_Position; }
        void SetPosition(const glm::lowp_vec2& position) { m_Position = position; }

        //! \brief Get the mass of the particle.
        float GetMass() const { return m_Mass; }
        void SetMass(float mass) { m_Mass = mass; }

        static float Radius; //! The default radius of the particle. (13.0f)

    private:
        glm::lowp_vec2 m_Position; //! The position of the particle.
        float m_Mass; //! The mass of the particle.
};