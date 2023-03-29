#pragma once

#include <glm/glm.hpp>

// This file is used to define the particle VAO
// It replaced the previous (ParticleAdapter.cpp)
namespace Particle_OPENGL
{
    //! \brief Initialize the VAO and VBO for the particle.
    //! The purpose is to reduce as much as possible the computation needed to draw the particle.
    void LoadParticleVAO();

    extern unsigned int VAO;
    extern unsigned int VBO;
    extern float* vertices;
    extern int nbVertices;
};

// alternative particle adapter for optimization
struct ParticleStruct
{
    float pos_x;
    float pos_y;
    float mass;
};