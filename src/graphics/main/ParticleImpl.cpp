#include "ParticleImpl.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <iostream>

float particleRadius = 5.0f;

namespace Particle_OPENGL
{
    unsigned int pVAO = 0;
    unsigned int pVBO = 0;
    float* pvertices = nullptr;
    int pnbVertices = 0;

    void LoadParticleVAO(unsigned int& VAO, unsigned int& VBO, float*& vertices, int& nbVertices, float radius)
    {
        if(vertices != nullptr)
        {
            nbVertices = 0;
            delete[] vertices;
            glDeleteBuffers(1, &VBO);
            glDeleteVertexArrays(1, &VAO);
        }

        // Create the VAO and VBO for the particle.

        // Calculate the number of vertices depending on the radius (for a better circle)
        // More vertices as the circle is bigger.
        // using Trigonometry cos(theAngleWeWant) because we know the radius (hypotenuse) and the adjacent side (hypotenuse - precision)
        float angle = glm::degrees(acos((radius - 1.0f) / radius));
        nbVertices = (int)(360.0f / angle);

        // Allocate memory for the vertices (x, y)
        vertices = new float[nbVertices * 2];

        for(int i = 0; i < nbVertices; i++)
        {
            // Calculate the angle in radians
            float rad = glm::radians(angle * i);

            // Calculate the x and y position from origin (0, 0)
            // 0 + cos(rad) * radius;
            // 0 = center of the circle (it will be moved later in the shader)
            float x = cos(rad) * radius;
            float y = sin(rad) * radius;

            // Set the position
            vertices[i*2 + 0] = x;
            vertices[i*2 + 1] = y;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, nbVertices * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
}

