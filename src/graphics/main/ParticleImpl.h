#pragma once

#include <glm/glm.hpp>

enum ParticleClass {
    PART_CLASS_1,
    PART_CLASS_2,
    PART_CLASS_3
};

// Particle radius
extern float particleRadius;

// This file is used to define the particle VAO
// It replaced the previous (ParticleAdapter.cpp)
namespace Particle_OPENGL
{
    extern unsigned int pVAO;
    extern unsigned int pVBO;
    extern float* pvertices;
    extern int pnbVertices;

    //! \brief Initialize the VAO and VBO for the particle.
    //! The purpose is to reduce as much as possible the computation needed to draw the particle.
    void LoadParticleVAO(unsigned int& VAO = Particle_OPENGL::pVAO, unsigned int& VBO = Particle_OPENGL::pVBO,
                         float*& vertices = Particle_OPENGL::pvertices, int& nbVertices = Particle_OPENGL::pnbVertices, float radius = particleRadius);

    
};

// alternative particle adapter for optimization
struct ParticleStruct
{
    float pos_x;
    float pos_y;
    float mass;
};

// This struct is used in GraphicContext to define the Behavior between particles.
struct ParticleBehavior
{
    float repulsion;
    float force;
    float repulsion_distance;
    float force_distance;
    
    ParticleBehavior(float repulsion, float force, float repulsion_distance, float force_distance)
    {
        this->repulsion = repulsion;
        this->force = force;
        this->repulsion_distance = repulsion_distance;
        this->force_distance = force_distance;
    }

    ParticleBehavior()
    {
        this->repulsion = 0.0f;
        this->force = 0.0f;
        this->repulsion_distance = 0.0f;
        this->force_distance = 0.0f;
    }
    
    // Setup a few preconfigured behaviors
    void Repulsion();
    void Attraction();
    void Small_Repulsion();
    void Small_Attraction();
    void None();



};

