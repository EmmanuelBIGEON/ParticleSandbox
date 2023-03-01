#include "ParticleAdapter.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../main/GraphicContext.h"

unsigned int ParticleAdapter::VAO = 0;
unsigned int ParticleAdapter::VBO = 0;

float* ParticleAdapter::vertices = nullptr;
int ParticleAdapter::nbVertices = 0;

glm::vec3 ParticleAdapter::highlightColor = glm::vec3(0.9, 0.01, 0.01);

ParticleAdapter::ParticleAdapter(GraphicContext* context, Particle* particle) 
    : GraphicObject(context, AvailableShader::SHADER_PARTICLE, OBJECTGR_PARTICLE), 
    m_Particle(particle), m_Color(0.05, 0.9, 0.91), m_Highlight(false), mvt_x(0.0f), mvt_y(0.0f)
{
    m_Shader = context->GetShader(AvailableShader::SHADER_PARTICLE);
    m_Position  = particle->GetPosition();
}

ParticleAdapter::~ParticleAdapter()
{
}

void ParticleAdapter::Update()
{
    mvt_x = 0.0f;
    mvt_y = 0.0f;
    glm::lowp_vec2 resulting_movement = glm::lowp_vec2(0.0f, 0.0f);
    // For all particles in context
    for(const auto& particle : m_Context->GetParticleAdapters())
    {
        if(particle == this)
            continue;

        float distance = glm::distance(m_Position, particle->m_Position);
        
        
        if(distance > 0.0f && distance < 5.0f)
        {
            // repulsion
            glm::lowp_vec2 direction = glm::normalize(m_Position - particle->m_Position);
            resulting_movement += 0.2f * direction / distance;
            mvt_x = resulting_movement.x;
            mvt_y = resulting_movement.y;
            continue;
        }else if(distance < 100.0f)
        {
            continue;
            // do nothing.
        }else if(distance < 250.0f)
        {
            // Simplified attraction to keep the particles together
            glm::lowp_vec2 direction = glm::normalize(particle->m_Position - m_Position);
            resulting_movement += direction / (distance);
            mvt_x += resulting_movement.x;
            mvt_y += resulting_movement.y;
        }else
        {

            // Use the law of gravitation to compute the force
            // formula = F = G * ((m1 * m2) / r^2)
            // Let's assume G = 1
            glm::lowp_vec2 direction = glm::normalize(particle->m_Position - m_Position);
            float force = 40* (m_Particle->GetMass() * particle->m_Particle->GetMass()) / (distance * distance);
            mvt_x += direction.x * force;
            mvt_y += direction.y * force;
        }
    }
    
    // apply coef to accelerate the simulation
    mvt_x *= 6.0f;
    mvt_y *= 6.0f;
    m_Position += glm::lowp_vec2(mvt_x, mvt_y);
    // m_Particle->SetPosition(position);
}

void ParticleAdapter::Draw()
{
    m_Shader->SetVec3("particleColor", m_Highlight ? highlightColor : m_Color);
    m_Shader->SetVec2("shiftPos", m_Position);
    glDrawArrays(GL_TRIANGLE_FAN, 0, nbVertices);

}


void ParticleAdapter::LoadParticleVAO()
{
    if(vertices != nullptr)
    {
        nbVertices = 0;
        delete[] vertices;

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    // Create the VAO and VBO for the particle.
    // Default size is 10.0f radius.
    float radius = Particle::Radius;

    // Calculate the number of vertices depending on the radius (for a better circle)
    // More vertices as the circle is bigger.
    // using Trigonometry cos(theAngleWeWant) because we know the radius (hypotenuse) and the adjacent side (hypotenuse - precision)
    float angle = glm::degrees(acos((radius - 1.0f) / radius));
    nbVertices = (int)(360.0f / angle);

    // Allocate memory for the vertices
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