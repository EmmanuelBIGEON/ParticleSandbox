#include "ParticleAdapter.h"

#include "../../main/ParticleImpl.h"
#include "../../main/Shader.h"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

unsigned int ParticleAdapter::VAO = 0;
unsigned int ParticleAdapter::VBO = 0;
float* ParticleAdapter::vertices = nullptr;
int ParticleAdapter::nbVertices = 0;


ParticleAdapter::ParticleAdapter(GraphicContext* context, const glm::vec2& position, const glm::vec3& color, float size) : GraphicObject(context, SHADER_PARTICLE),
    m_Position(position), m_Color(color), m_Size(size)
{
    m_Shader = context->GetShader(SHADER_PARTICLE);
}

ParticleAdapter::~ParticleAdapter()
{
}

void ParticleAdapter::Update()
{
    Particle_OPENGL::LoadParticleVAO(VAO, VBO, vertices, nbVertices, m_Size);
    m_IsUpdated = true;
}

void ParticleAdapter::Draw()
{
    m_Shader->SetVec3("particleColor", m_Color);
    glBindVertexArray(ParticleAdapter::VAO);
    m_Shader->SetVec2("shiftPos", m_Position);
    glDrawArrays(GL_TRIANGLE_FAN, 0, nbVertices);
    
}
