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

#include <immintrin.h>


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

    // setup particleadapter3 handling
    m_ParticleAdapters3_posX = nullptr;
    m_ParticleAdapters3_posY = nullptr;
    m_ParticleAdapters3_mass = nullptr;
    nb_ParticleAdapters3 = 0;
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

    
    shader_particle->Use();
    shader_particle->SetVec3("particleColor", glm::vec3(0.21, 0.41, 0.91));
    glBindVertexArray(ParticleAdapter::VAO);

    float repulsion_factor = 0.0025f; // You can adjust this value
    float attraction_factor = 0.0001f; // You can adjust this value
    for(int i = 0; i < nb_ParticleAdapters3; i++)
    {
        float mvt_x = 0.0f;
        float mvt_y = 0.0f;

        // // load scalar (current x and y)
        float xvalue = m_ParticleAdapters3_posX[i];
        float yvalue = m_ParticleAdapters3_posY[i];
        __m256 scalar_x = _mm256_set1_ps(xvalue);
        __m256 scalar_y = _mm256_set1_ps(yvalue);

        for(int j = 0; j < nb_ParticleAdapters3; j+=8)
        {
            // load vector (other x and y)
            __m256 vecX = _mm256_load_ps(&m_ParticleAdapters3_posX[j]);
            __m256 vecY = _mm256_load_ps(&m_ParticleAdapters3_posY[j]);
            // Calculate the distance between the current particle and all the other
            __m256 sub_x = _mm256_sub_ps(vecX, scalar_x);
            __m256 sub_y = _mm256_sub_ps(vecY, scalar_y);
            __m256 mask_x = _mm256_cmp_ps(sub_x, _mm256_set1_ps(800.0f), _CMP_GT_OQ);
            __m256 mask_y = _mm256_cmp_ps(sub_y, _mm256_set1_ps(600.0f), _CMP_GT_OQ);
            __m256 mask_x2 = _mm256_cmp_ps(sub_x, _mm256_set1_ps(-800.0f), _CMP_LT_OQ);
            __m256 mask_y2 = _mm256_cmp_ps(sub_y, _mm256_set1_ps(-600.0f), _CMP_LT_OQ);
            __m256 other_x = _mm256_blendv_ps(vecX, _mm256_sub_ps(vecX, _mm256_set1_ps(1600.0f)), mask_x);
            __m256 other_y = _mm256_blendv_ps(vecY, _mm256_sub_ps(vecY, _mm256_set1_ps(1200.0f)), mask_y);
            other_x = _mm256_blendv_ps(other_x, _mm256_add_ps(other_x, _mm256_set1_ps(1600.0f)), mask_x2);
            other_y = _mm256_blendv_ps(other_y, _mm256_add_ps(other_y, _mm256_set1_ps(1200.0f)), mask_y2);
            sub_x = _mm256_sub_ps(other_x, scalar_x);
            sub_y = _mm256_sub_ps(other_y, scalar_y);
            __m256 distance = _mm256_sqrt_ps(_mm256_add_ps(_mm256_mul_ps(sub_x, sub_x), _mm256_mul_ps(sub_y, sub_y)));
            // // Calculate the direction between the current particle and all the others
            __m256 mask_repulsion = _mm256_cmp_ps(distance, _mm256_set1_ps(1000.0f), _CMP_LT_OQ);
            // Calculate the direction between the current particle and all the others
            __m256 mask_direction = _mm256_cmp_ps(distance, _mm256_setzero_ps(), _CMP_EQ_OQ);
            __m256 direction_x = _mm256_blendv_ps(_mm256_div_ps(sub_x, distance), _mm256_set1_ps(0.0f), mask_direction);
            __m256 direction_y = _mm256_blendv_ps(_mm256_div_ps(sub_y, distance), _mm256_set1_ps(0.0f), mask_direction);
            // invert direction if the particle is too close
            __m256 mask_invert = _mm256_cmp_ps(distance, _mm256_set1_ps(200.0f), _CMP_LT_OQ);
            direction_x = _mm256_blendv_ps(direction_x, _mm256_mul_ps(direction_x, _mm256_set1_ps(-1.0f)), mask_invert);
            direction_y = _mm256_blendv_ps(direction_y, _mm256_mul_ps(direction_y, _mm256_set1_ps(-1.0f)), mask_invert);
            // normalize direction to an unit vector
            __m256 direction_norm = _mm256_sqrt_ps(_mm256_add_ps(_mm256_mul_ps(direction_x, direction_x), _mm256_mul_ps(direction_y, direction_y)));
            __m256 mask_norm = _mm256_cmp_ps(direction_norm, _mm256_setzero_ps(), _CMP_EQ_OQ);
            direction_x = _mm256_blendv_ps(_mm256_div_ps(direction_x, direction_norm), _mm256_set1_ps(0.0f), mask_norm);
            direction_y = _mm256_blendv_ps(_mm256_div_ps(direction_y, direction_norm), _mm256_set1_ps(0.0f), mask_norm);

            // Calculate the movement of the current particle, if attraction or repulsion depending
            // on the distance between the current particle and the others
            __m256 mask_attraction = _mm256_cmp_ps(distance, _mm256_set1_ps(1000.0f), _CMP_GT_OQ);
            __m256 attraction_x = _mm256_blendv_ps(_mm256_mul_ps(direction_x, _mm256_set1_ps(attraction_factor)), _mm256_set1_ps(0.0f), mask_attraction);
            __m256 attraction_y = _mm256_blendv_ps(_mm256_mul_ps(direction_y, _mm256_set1_ps(attraction_factor)), _mm256_set1_ps(0.0f), mask_attraction);

            // calculate repulsion 
            __m256 repulsion_x = _mm256_blendv_ps(_mm256_mul_ps(sub_x, _mm256_set1_ps(repulsion_factor)), _mm256_set1_ps(0.0f), mask_repulsion);
            __m256 repulsion_y = _mm256_blendv_ps(_mm256_mul_ps(sub_y, _mm256_set1_ps(repulsion_factor)), _mm256_set1_ps(0.0f), mask_repulsion);

            // // resulting movement
            __m256 mvt_x_tmp = _mm256_add_ps(attraction_x, repulsion_x);
            __m256 mvt_y_tmp = _mm256_add_ps(attraction_y, repulsion_y);

            float* mvt_x_tmp_ptr = (float*)&mvt_x_tmp;
            float* mvt_y_tmp_ptr = (float*)&mvt_y_tmp;
            for(int k = 0; k < 8; k++)
            {
                mvt_x += mvt_x_tmp_ptr[k];
                mvt_y += mvt_y_tmp_ptr[k];
            }
        }

        m_ParticleAdapters3_posX[i] += mvt_x*10.0f;
        m_ParticleAdapters3_posY[i] += mvt_y*10.0f;

        // // Check if the particle is out of the screen
        if(m_ParticleAdapters3_posX[i] < 0.0f)
            m_ParticleAdapters3_posX[i] = m_ParticleAdapters3_posX[i] + 1600.0f;
        else if(m_ParticleAdapters3_posX[i] > 1600.0f)
            m_ParticleAdapters3_posX[i] = m_ParticleAdapters3_posX[i] - 1600.0f;
        if(m_ParticleAdapters3_posY[i] < 0.0f)
            m_ParticleAdapters3_posY[i] = m_ParticleAdapters3_posY[i] + 1200.0f;
        else if(m_ParticleAdapters3_posY[i] > 1200.0f)
            m_ParticleAdapters3_posY[i] = m_ParticleAdapters3_posY[i] - 1200.0f;

        shader_particle->SetVec2("shiftPos", glm::vec2(m_ParticleAdapters3_posX[i], m_ParticleAdapters3_posY[i]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, ParticleAdapter::nbVertices);
    }

    

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
    // if(m_ParticleAdapters2 == nullptr)
    // {
    //     m_ParticleAdapters2 = new ParticleAdapter2[nbParticles];
    //     len_ParticleAdapters2 = nbParticles;
    //     m_ParticleAdapters2_begin = m_ParticleAdapters2;
    //     m_ParticleAdapters2_end = m_ParticleAdapters2 + len_ParticleAdapters2;

    //     // fill with random values between world bounds
    //     for(ParticleAdapter2* adapter = m_ParticleAdapters2_begin; adapter < m_ParticleAdapters2_end; adapter++)
    //     {
    //         adapter->pos_x = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1500.0f - 50.0f)));
    //         adapter->pos_y = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1100.0f - 50.0f)));
    //         adapter->mass = 1.0f;
    //     }
    // }else 
    // {
    //     // Recreate the array with the new size
    //     ParticleAdapter2* oldArray = m_ParticleAdapters2;
    //     m_ParticleAdapters2 = new ParticleAdapter2[nbParticles + len_ParticleAdapters2];
    //     memcpy(m_ParticleAdapters2, oldArray, len_ParticleAdapters2 * sizeof(ParticleAdapter2));
    //     len_ParticleAdapters2 += nbParticles;
    //     delete[] oldArray;
    //     m_ParticleAdapters2_begin = m_ParticleAdapters2;
    //     m_ParticleAdapters2_end = m_ParticleAdapters2 + len_ParticleAdapters2;

    //     // fill with random values between world bounds
    //     ParticleAdapter2* start = m_ParticleAdapters2 + len_ParticleAdapters2 - nbParticles;
    //     for(ParticleAdapter2* adapter = start; adapter < m_ParticleAdapters2_end; adapter++)
    //     {
    //         adapter->pos_x = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1500.0f - 50.0f)));
    //         adapter->pos_y = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1100.0f - 50.0f)));
    //         adapter->mass = 0;
    //     }
    // }

    // // print first teen values of the array
    // for(int i = 0; i < 10; i++)
    // {
    //     std::cout << "pos_x: " << m_ParticleAdapters2[i].pos_x << " pos_y: " << m_ParticleAdapters2[i].pos_y << std::endl;
    // }

    // third version..
    // Using SIMD
    // Need to fill arrays of posx posy and mass
    // forcing a multiple of 8 for the number of particles
    if(nbParticles % 8 != 0)
    {
        std::cout << "Multiple of 8 required" << std::endl;
        return;
    }

    if(nb_ParticleAdapters3)
    {

        // Recreate the different arrays with the new size in memory, don't forget to delete the old ones and to save their values into the new one.
        // Last size = nb_ParticleAdapters3
        float* old_pos_x = m_ParticleAdapters3_posX;
        float* old_pos_y = m_ParticleAdapters3_posY;
        float* old_mass = m_ParticleAdapters3_mass;

        m_ParticleAdapters3_posX = new float[nb_ParticleAdapters3 + nbParticles];
        m_ParticleAdapters3_posY = new float[nb_ParticleAdapters3 + nbParticles];
        m_ParticleAdapters3_mass = new float[nb_ParticleAdapters3 + nbParticles];

        memcpy(m_ParticleAdapters3_posX, old_pos_x, nb_ParticleAdapters3 * sizeof(float));
        memcpy(m_ParticleAdapters3_posY, old_pos_y, nb_ParticleAdapters3 * sizeof(float));
        memcpy(m_ParticleAdapters3_mass, old_mass, nb_ParticleAdapters3 * sizeof(float));

        delete[] old_pos_x;
        delete[] old_pos_y;
        delete[] old_mass;

        // fill with random values between world bounds
        for(int i = nb_ParticleAdapters3; i < nb_ParticleAdapters3 + nbParticles; i++)
        {
            m_ParticleAdapters3_posX[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1600.0f)));
            m_ParticleAdapters3_posY[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1200.0f)));
            m_ParticleAdapters3_mass[i] = 1.0f;
        }
    }else
    {
        // Allocate the memory for the particles
        m_ParticleAdapters3_posX = new float[nbParticles];
        m_ParticleAdapters3_posY = new float[nbParticles];
        m_ParticleAdapters3_mass = new float[nbParticles];

        // fill with random values between world bounds
        for(int i = 0; i < nbParticles; i++)
        {
            m_ParticleAdapters3_posX[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1600.0f)));
            m_ParticleAdapters3_posY[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1200.0f)));
            m_ParticleAdapters3_mass[i] = 1.0f;
        }

        nb_ParticleAdapters3 = nbParticles;
    }

    // print the firest 10 points
    for(int i = 0; i < 10; i++)
    {
        std::cout << "pos_x: " << m_ParticleAdapters3_posX[i] << " pos_y: " << m_ParticleAdapters3_posY[i] << std::endl;
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

