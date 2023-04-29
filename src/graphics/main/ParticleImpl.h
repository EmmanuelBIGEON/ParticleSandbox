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
    
    ParticleBehavior(float repulsion, float attraction, float repulsion_distance, float attraction_distance)
    {
        this->repulsion = repulsion;
        this->attraction = attraction;
        this->repulsion_distance = repulsion_distance;
        this->attraction_distance = attraction_distance;
    }

    ParticleBehavior()
    {
        this->repulsion = 0.0f;
        this->attraction = 0.0f;
        this->repulsion_distance = 0.0f;
        this->attraction_distance = 0.0f;
    }
    
    float repulsion;
    float attraction;
    float repulsion_distance;
    float attraction_distance;

        
    // Setup a few preconfigured behaviors
    void Repulsion();
    void Attraction();
    void None();



};

