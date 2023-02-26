#include "ParticleDetector.h"

#include <glm/glm.hpp>

#include <iostream>

ParticleDetector::ParticleDetector(GraphicContext* context, const glm::lowp_vec2& center, float radius)
    : m_Context(context), m_Center(center), m_Radius(radius)
{
}

ParticleDetector::~ParticleDetector()
{
}

void ParticleDetector::Update()
{
    std::vector<ParticleAdapter*> newParticles;
    std::vector<ParticleAdapter*> erasedParticles;

    for (auto& object : m_Context->GetObjects())
    {
        if (object->GetObjectType() == OBJECTGR_PARTICLE)
        {
            ParticleAdapter* particle = static_cast<ParticleAdapter*>(object);
            
            // if(glm::distance(particle->GetParticle()->GetPosition(), m_Center) < m_Radius)
            // {
            //     // if not in list
            //     if(std::find(m_Particles.begin(), m_Particles.end(), particle) == m_Particles.end())
            //     {
            //         m_Particles.push_back(particle);
            //         newParticles.push_back(particle);
            //     }
            // }else
            // {
            //     // if in list
            //     if(std::find(m_Particles.begin(), m_Particles.end(), particle) != m_Particles.end())
            //     {
            //         m_Particles.erase(std::remove(m_Particles.begin(), m_Particles.end(), particle), m_Particles.end());
            //         erasedParticles.push_back(particle);
            //     }
            // }
            
            if(std::find(m_Particles.begin(), m_Particles.end(), particle) == m_Particles.end())
            {
                if (glm::length(particle->GetParticle()->GetPosition() - m_Center) < m_Radius)
                {
                    m_Particles.push_back(particle); 
                    newParticles.push_back(particle);
                }
            }else 
            {
                
                // Particle is in the list
                if(glm::length(particle->GetParticle()->GetPosition() - m_Center) >= m_Radius)
                {
                    m_Particles.erase(std::remove(m_Particles.begin(), m_Particles.end(), particle), m_Particles.end());
                    erasedParticles.push_back(particle);
                }
            }
        }
    }

    if (newParticles.size() > 0)
    {
        OnNewParticles.Emit(newParticles);
    }

    if (erasedParticles.size() > 0)
    {
        OnErasedParticles.Emit(erasedParticles);
    }
}
