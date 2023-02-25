#include "ParticleAdapter.h"

#include <glad/glad.h>

unsigned int ParticleAdapter::VAO = 0;
unsigned int ParticleAdapter::VBO = 0;

float* ParticleAdapter::vertices = nullptr;
int ParticleAdapter::nbVertices = 0;

glm::vec3 ParticleAdapter::highlightColor = glm::vec3(0.9, 0.01, 0.01);

ParticleAdapter::ParticleAdapter(GraphicContext* context, Particle* particle) 
    : GraphicObject(context, AvailableShader::SHADER_PARTICLE), m_Particle(particle), m_Color(0.05, 0.9, 0.91), m_Highlight(false)
{
    m_ObjectType = OBJECTGR_PARTICLE;
    m_Shader = context->GetShader(AvailableShader::SHADER_PARTICLE);
}

ParticleAdapter::~ParticleAdapter()
{
}

void ParticleAdapter::Update()
{
    // Will come later.
    // Collisions and stuff. behaviour will be defined in this method
}

void ParticleAdapter::Draw()
{
    // std::cout << "Drawing particle" << std::endl;
    glm::vec2 position = m_Particle->GetPosition();
    m_Shader->SetVec2("shiftPos", position);
    if(m_Highlight)
    {
        m_Shader->SetVec3("particleColor", highlightColor);
    }
    else
    {
        m_Shader->SetVec3("particleColor", m_Color);
    }

    // std::cout << "Drawing particle position: " << position.x << ", " << position.y << std::endl;
    glBindVertexArray(VAO);
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
    float radius = 10.0f;

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