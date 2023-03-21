#include "GraphicContext.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

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

#ifndef __EMSCRIPTEN__
#include <thread>
#endif




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

    // setup particleadapter2 handling
    m_ParticleAdapters2 = nullptr;
    sizestruct_ParticleAdapters2 = sizeof(ParticleAdapter2);
    len_ParticleAdapters2 = 0;
    m_ParticleAdapters2_begin = nullptr;
    m_ParticleAdapters2_end = nullptr;
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
    // SHADER_BUTTON
    shader_button = new Shader("shaders/button.vs", "shaders/button.fs");
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
                {
                    object->Update();   

                }

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
            case SHADER_BUTTON:
            {
                shader_button->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
        }
        object->Draw();
    }

    // render particels
    // shader_particle->Use();
    // shader_particle->SetVec3("particleColor", ParticleAdapter::highlightColor);
    // glBindVertexArray(ParticleAdapter::VAO);

    // // Chrono chrono;
    // // chrono.Start();
    // // if(m_ParticleAdapters.size() > 300)
    // // {
    // //     std::thread t1(&GraphicContext::RenderThread, this, 2, 0);
    // //     std::thread t2(&GraphicContext::RenderThread, this, 2, 1);

    // //     t1.join();
    // //     t2.join();
    // //     for(auto adapter : m_ParticleAdapters) adapter->Draw();
    // // }else
    // // {
    // //     for(auto adapter : m_ParticleAdapters)
    // //     {
    // //         adapter->Update();
    // //         adapter->Draw();
    // //     }
    // // }

    // for(auto adapter : m_ParticleAdapters)
    // {
    //     adapter->Update();
    //     adapter->Draw();
    // }

    // render particles v2
    shader_particle->Use();
    shader_particle->SetVec3("particleColor", glm::vec3(0.9, 0.01, 0.01));
    glBindVertexArray(ParticleAdapter::VAO);

    // Chrono chrono;
    // double timerender_distancecalculation = 0.0;
    // double timerender_mvtcalculation = 0.0;
    // double timerender_preprocess = 0.0;
    // double timerender_afterprocess = 0.0;
    

    for(ParticleAdapter2* adapter = m_ParticleAdapters2_begin; adapter != m_ParticleAdapters2_end; adapter++)
    {
        // SUPER EFFICIENT COMPUTE
        ParticleAdapter2* cursor = m_ParticleAdapters2_begin;
        float mvt_x = 0.0f;
        float mvt_y = 0.0f;
        while(cursor != m_ParticleAdapters2_end)
        {
            if(cursor == adapter)
            {
                cursor++;
                continue;
            }
            // chrono.Start();
            // Computation of particles
            const float& vect_x = cursor->pos_x - adapter->pos_x;
            const float& vect_y = cursor->pos_y - adapter->pos_y;
            // chrono.Stop();
            // timerender_preprocess += chrono.GetElapsedTime();

            // if(vect_x > 800.0f)
            //     other_x -= 1600.0f;
            // else if(vect_x < -800.0f)
            //     other_x += 1600.0f;

            // if(vect_y > 600.0f)
            //     other_y -= 1200.0f;
            // else if(vect_y < -600.0f)
            //     other_y += 1200.0f;

            // chrono.Start();
            float distance = sqrt(vect_x*vect_x + vect_y*vect_y);
            // chrono.Stop();
            // timerender_distancecalculation += chrono.GetElapsedTime();

            // chrono.Start();
            const float& direction_x = vect_x / distance;
            const float& direction_y = vect_y / distance;

            if(distance < 100.0f)
            {
                mvt_x += direction_x * (100.0f - distance) / 100.0f;
                mvt_y += direction_y * (100.0f - distance) / 100.0f;
            }else 
            {
                mvt_x += direction_x * 0.1f;
                mvt_y += direction_y * 0.1f;
            }
            // chrono.Stop();
            // timerender_mvtcalculation += chrono.GetElapsedTime();

            cursor++;
        }

        // chrono.Start();
        adapter->pos_x += mvt_x;
        adapter->pos_y += mvt_y;
        
        if(adapter->pos_x < 0.0f)
            adapter->pos_x = adapter->pos_x + 1600.0f;
        else if(adapter->pos_x > 1600.0f)
            adapter->pos_x = adapter->pos_x - 1600.0f;
        if(adapter->pos_y < 0.0f)
            adapter->pos_y = adapter->pos_y + 1200.0f;
        else if(adapter->pos_y > 1200.0f)
            adapter->pos_y = adapter->pos_y - 1200.0f;
        
        // chrono.Stop();
        // timerender_afterprocess += chrono.GetElapsedTime();

        shader_particle->SetVec2("shiftPos", glm::vec2(adapter->pos_x, adapter->pos_y));
        glDrawArrays(GL_TRIANGLE_FAN, 0, ParticleAdapter::nbVertices);

    }

    // display times
    // std::cout << "timerender_distancecalculation: " << timerender_distancecalculation << std::endl;
    // std::cout << "timerender_mvtcalculation: " << timerender_mvtcalculation << std::endl;
    // std::cout << "timerender_preprocess: " << timerender_preprocess << std::endl;
    // std::cout << "timerender_afterprocess: " << timerender_afterprocess << std::endl;
    // std::cout << "timerender_total: " << timerender_distancecalculation + timerender_mvtcalculation + timerender_preprocess + timerender_afterprocess << std::endl;
    // std::cout << std::endl;

}

void GraphicContext::Update()
{
    // Shader_basic
    shader_basic->Use();
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
    // Update the model matrix
    shader_particle->SetMat4("model", m_ModelMatrix);

    // shader_button
    shader_button->Use();
    // Update the model matrix
    shader_button->SetMat4("model", m_ModelMatrix);

    needUpdate = false;
}


void GraphicContext::AddParticles(int nbParticles)
{
    // m_ParticleAdapters2
    // Allocate the memory for the particles
    if(m_ParticleAdapters2 == nullptr)
    {
        m_ParticleAdapters2 = new ParticleAdapter2[nbParticles];
        len_ParticleAdapters2 = nbParticles;
        m_ParticleAdapters2_begin = m_ParticleAdapters2;
        m_ParticleAdapters2_end = m_ParticleAdapters2 + len_ParticleAdapters2;

        // fill with random values between world bounds
        for(ParticleAdapter2* adapter = m_ParticleAdapters2_begin; adapter < m_ParticleAdapters2_end; adapter++)
        {
            adapter->pos_x = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1500.0f - 50.0f)));
            adapter->pos_y = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1100.0f - 50.0f)));
            adapter->mass = 1.0f;
        }
    }else 
    {
        // Recreate the array with the new size
        ParticleAdapter2* oldArray = m_ParticleAdapters2;
        m_ParticleAdapters2 = new ParticleAdapter2[nbParticles + len_ParticleAdapters2];
        memcpy(m_ParticleAdapters2, oldArray, len_ParticleAdapters2 * sizeof(ParticleAdapter2));
        len_ParticleAdapters2 += nbParticles;
        delete[] oldArray;
        m_ParticleAdapters2_begin = m_ParticleAdapters2;
        m_ParticleAdapters2_end = m_ParticleAdapters2 + len_ParticleAdapters2;

        // fill with random values between world bounds
        ParticleAdapter2* start = m_ParticleAdapters2 + len_ParticleAdapters2 - nbParticles;
        for(ParticleAdapter2* adapter = start; adapter < m_ParticleAdapters2_end; adapter++)
        {
            adapter->pos_x = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1500.0f - 50.0f)));
            adapter->pos_y = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1100.0f - 50.0f)));
            adapter->mass = 0;
        }
    }

    // print first teen values of the array
    for(int i = 0; i < 10; i++)
    {
        std::cout << "pos_x: " << m_ParticleAdapters2[i].pos_x << " pos_y: " << m_ParticleAdapters2[i].pos_y << std::endl;
    }
    
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
        case SHADER_BUTTON:
            return shader_button;
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

