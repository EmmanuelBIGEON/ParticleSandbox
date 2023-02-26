#pragma once

#include <vector>

#include "../components/adapters/ParticleAdapter.h"
#include "../main/GraphicContext.h"

#include "../../common/Signal.h"

//! \class ParticleDetector
//! Detect particle in a given area.
class ParticleDetector
{
    public:
        ParticleDetector(GraphicContext* context, const glm::lowp_vec2& center, float radius);
        virtual ~ParticleDetector();

        //! \brief Update the list of particles in the area.
        void Update();

        //! \brief Get the list of particles in the area.
        const std::vector<ParticleAdapter*>& GetParticles() const { return m_Particles; }
        
        //! \brief Set a new center for the area.
        void SetCenter(const glm::lowp_vec2& center) { m_Center = center; }

        Signal<std::vector<ParticleAdapter*>> OnNewParticles;
        Signal<std::vector<ParticleAdapter*>> OnErasedParticles;

    private:
        GraphicContext* m_Context;
        glm::lowp_vec2 m_Center;
        float m_Radius;

        std::vector<ParticleAdapter*> m_Particles;
};