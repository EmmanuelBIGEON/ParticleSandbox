#include "GraphicContext.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../components/adapters/ParticleAdapter.h"

#include <iostream>

#include "GraphicObject.h"

#include "Shader.h"
#include "Font.h"
#include "../../common/Window.h"
#include "../../common/Chrono.h"

#include <thread>




float GraphicContext::worldWidth = 1600.0f;
float GraphicContext::worldHeight = 1200.0f;

GraphicContext::GraphicContext() 
    : okRendering(false), m_Objects(), needUpdate(true), m_ModelMatrix(), m_ProjectionMatrix(), m_ViewMatrix(), 
    shader_basic(nullptr), shader_text(nullptr), shader_texture(nullptr), shader_lighting(nullptr), shader_line(nullptr), shader_particle(nullptr), zoomFactor(1.0f)
{
    glm::mat4 model = glm::mat4(1.0f);
    // translate for the center of the screen (because OpenGL is in [-1, 1])
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));

    model = glm::scale(model, glm::vec3(1.0f/(worldWidth/2.0f), 1.0f/(worldHeight/2.0f), 1.0f));
    // glm::mat4 projection = glm::ortho(0.0f, worldWidth, 0.0f, worldHeight, -1.0f, 1.0f); // TODO big fail..
    glm::mat4 projection = glm::mat4(1.0f);
    // apply a scaling factor to the projection matrix to zoom in or out
    float zoomFactor = 0.5f; // example value, adjust as needed
    projection = glm::scale(projection, glm::vec3(zoomFactor, zoomFactor, 1.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f)); 

    m_ModelMatrix = model;
    m_ProjectionMatrix = projection;
    m_ViewMatrix = view;
}

GraphicContext::~GraphicContext()
{
}

void GraphicContext::Init()
{
    // ---------- SHADERS ----------
    // SHADER_BASIC
    shader_basic = new Shader("shaders/basic.vs", "shaders/basic.fs");
    // SHADER_TEXT
    shader_text = new Shader("shaders/text.vs", "shaders/text.fs");
    // SHADER_TEXTURE
    shader_texture = new Shader("shaders/texture.vs", "shaders/texture.fs");
    // SHADER_LIGHTING
    shader_lighting = new Shader("shaders/lighting.vs", "shaders/lighting.fs");
    // SHADER_LINE
    shader_line = new Shader("shaders/line.vs", "shaders/line.fs");
    // SHADER_PARTICLE
    shader_particle = new Shader("shaders/particle.vs", "shaders/particle.fs");
    // -----------------------------

    // ---------- Fonts ----------
    font_main = new Font("fonts/arial.ttf", 48);
    // ----------------------------

    okRendering = true;
}

void GraphicContext::Clear(bool DeleteObjects)
{
    if(DeleteObjects)
    {
        for(auto object : m_Objects)
        {
            delete object;
        }
    }

    m_Objects.clear();
}

void GraphicContext::Register(GraphicObject* object)
{
    if(object->GetObjectType() == OBJECTGR_PARTICLE)
    {
        ParticleAdapter* adapter = static_cast<ParticleAdapter*>(object);
        m_ParticleAdapters.push_back(adapter);
    }else
    {
        m_Objects.push_back(object);
    }
    OnObjectRegistered.Emit(object);
}


void GraphicContext::Remove(GraphicObject* object)
{
    if(object->GetObjectType() == OBJECTGR_PARTICLE)
    {
        for(auto it = m_ParticleAdapters.begin(); it != m_ParticleAdapters.end(); it++)
        {
            if(*it == object)
            {
                m_ParticleAdapters.erase(it);
                OnObjectRemoved.Emit(object);
                return;
            }
        }
    }
    else
    {
        for(auto it = m_Objects.begin(); it != m_Objects.end(); it++)
        {
            if(*it == object)
            {
                m_Objects.erase(it);
                OnObjectRemoved.Emit(object);
                return;
            }
        }
    }
}


float GraphicContext::Convert_glX_to_WorldX(float x) const
{
    return (x + 1.0f) * (worldWidth/2.0f);

}

float GraphicContext::Convert_glY_to_WorldY(float y) const
{
    return (y + 1.0f) * (worldHeight/2.0f);
}

void GraphicContext::Render()
{
    if(needUpdate)
        Update();

    if(!okRendering)
        return;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for(auto object : m_Objects)
    {
        switch(object->GetShaderIndex())
        {
            case SHADER_BASIC:
            {
                shader_basic->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
            case SHADER_TEXT:
            {
                shader_text->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
            case SHADER_TEXTURE:
            {
                shader_texture->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
            case SHADER_LIGHTING:
            {
                shader_lighting->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
            case SHADER_LINE:
            {
                shader_line->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
        }
        object->Draw();
    }

    // render particels
    shader_particle->Use();
    shader_particle->SetVec3("particleColor", ParticleAdapter::highlightColor);
    glBindVertexArray(ParticleAdapter::VAO);

    // Chrono chrono;
    // chrono.Start();
    if(m_ParticleAdapters.size() > 300)
    {
        std::thread t1(&GraphicContext::RenderThread, this, 2, 0);
        std::thread t2(&GraphicContext::RenderThread, this, 2, 1);

        t1.join();
        t2.join();
        for(auto adapter : m_ParticleAdapters) adapter->Draw();
    }else
    {
        for(auto adapter : m_ParticleAdapters)
        {
            adapter->Update();
            adapter->Draw();
        }
    }

}

void GraphicContext::Update()
{
    // Shader_basic
    shader_basic->Use();
    // Update the projection matrix
    shader_basic->SetMat4("projection", m_ProjectionMatrix);
    // Update the view matrix
    shader_basic->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_basic->SetMat4("model", m_ModelMatrix);

    // Shader_text
    shader_text->Use();
    // Update the view matrix
    shader_text->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_text->SetMat4("model", m_ModelMatrix);

    // Shader_texture
    shader_texture->Use();
    // Update the projection matrix
    shader_texture->SetMat4("projection", m_ProjectionMatrix);
    // Update the view matrix
    shader_texture->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_texture->SetMat4("model", m_ModelMatrix);
    
    // Shader_lighting [won't be reshapped, kept for testing purposes]
    shader_lighting->Use();
    // Update the projection matrix
    shader_lighting->SetMat4("projection", m_ProjectionMatrix);
    // Update the view matrix
    shader_lighting->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_lighting->SetMat4("model", m_ModelMatrix);
    // Update the objectColor
    shader_lighting->SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    // Update the lightColor
    shader_lighting->SetVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    // Update the lightPos
    shader_lighting->SetVec3("lightPos",  glm::vec3(0.0f, 0.0f, 3.0f));
    // Update the viewPos
    shader_lighting->SetVec3("viewPos",  glm::vec3(0.0f, 0.0f, 3.0f));

    // shader_line
    shader_line->Use();
    // Update the projection matrix
    shader_line->SetMat4("projection", m_ProjectionMatrix);
    // Update the view matrix
    shader_line->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_line->SetMat4("model", m_ModelMatrix);

    // shader_particle
    shader_particle->Use();
    // Update the projection matrix
    shader_particle->SetMat4("projection", m_ProjectionMatrix);
    // Update the model matrix
    shader_particle->SetMat4("model", m_ModelMatrix);

    needUpdate = false;
}

void GraphicContext::RenderThread(int nbOfThreads, int threadId)
{
    int size = m_ParticleAdapters.size();
    int firstObject = (size / nbOfThreads) * threadId;
    int maxRenderObject = (size / nbOfThreads) * (threadId + 1);

    if(threadId == nbOfThreads - 1)
    {
        size = size % nbOfThreads;
        maxRenderObject += size;
    }

    ParticleAdapter* adapter;
    for(int i = firstObject; i < maxRenderObject; i++)
    {
        adapter = m_ParticleAdapters[i];
        adapter->Update();   
    }
}

Shader* GraphicContext::GetShader(AvailableShader shader)
{
    switch(shader)
    {
        case SHADER_BASIC:
            return shader_basic;
        case SHADER_TEXT:
            return shader_text;
        case SHADER_TEXTURE:
            return shader_texture;
        case SHADER_LIGHTING:
            return shader_lighting;
        case SHADER_LINE:
            return shader_line;
        case SHADER_PARTICLE:
            return shader_particle;
    }
    return nullptr;
}

void GraphicContext::ZoomIn(float factor)
{
    return; // TODO
    // limit zoomFactor to 10.0f
    if(zoomFactor > 10.0f)
        return;
    
    zoomFactor += factor;
    // std::cout << "ZoomFactor: " << zoomFactor << std::endl;
    needUpdate = true;
}

void GraphicContext::ZoomOut(float factor)
{
    return; // TODO
    // limit zoomFactor to 0.1f
    if(zoomFactor < 0.1f)
        return;
    
    zoomFactor -= factor;
    
    needUpdate = true;
}

