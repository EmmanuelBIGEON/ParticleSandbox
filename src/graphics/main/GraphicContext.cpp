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
#include <mutex>


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
float GraphicContext::repulsion_factor = 0.1f;
float GraphicContext::attraction_factor = 0.1f;


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
    // SHADER_UI
    shader_ui = new Shader("shaders/ui.vs", "shaders/ui.fs");
    // -----------------------------

    // ---------- Fonts ----------
    font_main = new Font("fonts/arial.ttf", 28);
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

    // No depth testing to relieve the GPU, UI objects will be drawn after particles to appear on top.
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    shader_particle->Use();
    shader_particle->SetVec3("particleColor", glm::vec3(0.21, 0.41, 0.91));
    glBindVertexArray(ParticleAdapter::VAO);

    m_ParticleAdaptersMutex.lock();
    for(int i = 0; i < nb_ParticleAdapters3; i++)
    {
        float mvt_x = 0.0f;
        float mvt_y = 0.0f;

        // // load scalar (current x and y)
        float xvalue = m_ParticleAdapters3_posX[i];
        float yvalue = m_ParticleAdapters3_posY[i];
        __m256 scalar_x = _mm256_set1_ps(xvalue);
        __m256 scalar_y = _mm256_set1_ps(yvalue);

        // Do each configuration considering the repetition of the world
        // Careful, for each configuration, sometimes the configuration is within worldbounds, you can't apply it.
        // find other configurations that are outside worldbounds to simulate the repetition of the world.
        // Ignoring first configuration (scalar_x, scalar_y), it will be the first calculation.
        std::vector<std::pair<__m256, __m256>> vec_scalarXY_config;
        vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalar_y));

        __m256 scalarX_left_to_right = _mm256_add_ps(scalar_x, _mm256_set1_ps(worldWidth));
        __m256 scalarX_right_to_left = _mm256_sub_ps(scalar_x, _mm256_set1_ps(worldWidth));
        __m256 scalarY_bottom_to_top = _mm256_add_ps(scalar_y, _mm256_set1_ps(worldHeight));
        __m256 scalarY_top_to_bottom = _mm256_sub_ps(scalar_y, _mm256_set1_ps(worldHeight));

        if(xvalue < (worldWidth / 2))
        {
            if(yvalue < (worldHeight / 2))
            {
                // Bottom left corner.
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
            }
            else
            {
                // Top left corner.
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
            }
        }else 
        {
            if(yvalue < (worldHeight / 2))
            {
                // Bottom right corner.
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
            }
            else
            {
                // Top right corner.
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
            }
        }    
        
        int sizeConfig = vec_scalarXY_config.size();
        

        for(int j = 0; (j+8) <= nb_ParticleAdapters3; j+=8)
        {
            // load vector (other x and y)
            __m256 vecX = _mm256_load_ps(&m_ParticleAdapters3_posX[j]);
            __m256 vecY = _mm256_load_ps(&m_ParticleAdapters3_posY[j]);
            // Calculate the minimal distance considering the available configurations
            // init distance, sub_x, sub_y
            __m256 sub_x = _mm256_sub_ps(vecX, scalar_x);
            __m256 sub_y = _mm256_sub_ps(vecY, scalar_y);
            
            __m256 distance = _mm256_add_ps(_mm256_mul_ps(sub_x, sub_x), _mm256_mul_ps(sub_y, sub_y));

            for(int k= 0; k < sizeConfig; k++)
            {
                __m256 tempsub_x = _mm256_sub_ps(vecX, vec_scalarXY_config[k].first);
                __m256 tempsub_y = _mm256_sub_ps(vecY, vec_scalarXY_config[k].second);
                __m256 distance_temp = _mm256_add_ps(_mm256_mul_ps(tempsub_x, tempsub_x), _mm256_mul_ps(tempsub_y, tempsub_y));
                __m256 mask = _mm256_cmp_ps(distance_temp, distance, _CMP_LT_OQ);
                distance = _mm256_blendv_ps(distance, distance_temp, mask);
                sub_x = _mm256_blendv_ps(sub_x, tempsub_x, mask);
                sub_y = _mm256_blendv_ps(sub_y, tempsub_y, mask);
            }

            // sqrt
            __m256 mask_sqrt = _mm256_cmp_ps(distance, _mm256_set1_ps(0.0f), _CMP_GT_OQ); // prevent division by 0
            distance = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_sqrt_ps(distance), mask_sqrt);

            // // Calculate the direction between the current particle and all the others
            // Calculate the direction between the current particle and all the others normalisation of the vector
            // use mask to avoid division by 0
            __m256 direction_x = _mm256_blendv_ps(sub_x, _mm256_div_ps(sub_x, distance), mask_sqrt);
            __m256 direction_y = _mm256_blendv_ps(sub_y, _mm256_div_ps(sub_y, distance), mask_sqrt);

            // Calculate the movement of the current particle, if attraction or repulsion depending
            // on the distance between the current particle and the others
            __m256 mask_attraction = _mm256_cmp_ps(distance, _mm256_set1_ps(200.0f), _CMP_GT_OQ);
            __m256 attraction_x = _mm256_blendv_ps(_mm256_mul_ps(direction_x, _mm256_set1_ps(attraction_factor)), _mm256_set1_ps(0.0f), mask_attraction);
            __m256 attraction_y = _mm256_blendv_ps(_mm256_mul_ps(direction_y, _mm256_set1_ps(attraction_factor)), _mm256_set1_ps(0.0f), mask_attraction);

            // calculate repulsion 
            // invert direction
            __m256 mask_repulsion = _mm256_cmp_ps(distance, _mm256_set1_ps(100.0f), _CMP_LT_OQ);
            direction_x = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_x, _mm256_set1_ps(-1.0f)), mask_repulsion);
            direction_y = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_y, _mm256_set1_ps(-1.0f)), mask_repulsion);
            __m256 repulsion_x = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_x, _mm256_set1_ps(repulsion_factor)), mask_repulsion);
            __m256 repulsion_y = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_y, _mm256_set1_ps(repulsion_factor)), mask_repulsion);

            // // resulting movement mask
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

        // calculate the rest of the impact of the other particles
        // for(int i = 8*(nb_ParticleAdapters3/8); i < nb_ParticleAdapters3; i++)
        // {
        //     // TODO
        //     // std::cout << "TODO" << std::endl;
        // }

        m_ParticleAdapters3_posX[i] += mvt_x;
        m_ParticleAdapters3_posY[i] += mvt_y;

        // // Check if the particle is out of the screen
        if(m_ParticleAdapters3_posX[i] < 0.0f)
            m_ParticleAdapters3_posX[i] = m_ParticleAdapters3_posX[i] + 1600.0f;
        else if(m_ParticleAdapters3_posX[i] >= 1600.0f)
            m_ParticleAdapters3_posX[i] = m_ParticleAdapters3_posX[i] - 1600.0f;
        if(m_ParticleAdapters3_posY[i] < 0.0f)
            m_ParticleAdapters3_posY[i] = m_ParticleAdapters3_posY[i] + 1200.0f;
        else if(m_ParticleAdapters3_posY[i] >= 1200.0f)
            m_ParticleAdapters3_posY[i] = m_ParticleAdapters3_posY[i] - 1200.0f;

        // std::cout << "drawing at " << m_ParticleAdapters3_posX[i] << " " << m_ParticleAdapters3_posY[i] << std::endl;
        shader_particle->SetVec2("shiftPos", glm::vec2(m_ParticleAdapters3_posX[i], m_ParticleAdapters3_posY[i]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, ParticleAdapter::nbVertices);
    }

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
            }case SHADER_UI:
            {
                shader_ui->Use();
                if(!object->IsUpdated()) // if the object is not updated, update it
                    object->Update();   

                break;
            }
        }
        object->Draw();
    }



    m_ParticleAdaptersMutex.unlock();
}

void GraphicContext::AddParticles(int nbParticles)
{
    // mutex
    m_ParticleAdaptersMutex.lock();
    // if(nbParticles % 8 != 0)
    // {
    //     std::cout << "Multiple of 8 required" << std::endl;
    //     return;
    // }

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

        nb_ParticleAdapters3 += nbParticles;
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

    OnParticlesAdded.Emit(nbParticles);

    m_ParticleAdaptersMutex.unlock();
}

void GraphicContext::AddParticles(const std::vector<ParticleStruct>& particles)
{
    m_ParticleAdaptersMutex.lock();
    int size = particles.size();
    if(size == 0)
        return;

    if(nb_ParticleAdapters3)
    {
        // Recreate the different arrays with the new size in memory, don't forget to delete the old ones and to save their values into the new one.
        // Last size = nb_ParticleAdapters3
        float* old_pos_x = m_ParticleAdapters3_posX;
        float* old_pos_y = m_ParticleAdapters3_posY;
        float* old_mass = m_ParticleAdapters3_mass;

        m_ParticleAdapters3_posX = new float[nb_ParticleAdapters3 + size];
        m_ParticleAdapters3_posY = new float[nb_ParticleAdapters3 + size];
        m_ParticleAdapters3_mass = new float[nb_ParticleAdapters3 + size];

        memcpy(m_ParticleAdapters3_posX, old_pos_x, nb_ParticleAdapters3 * sizeof(float));
        memcpy(m_ParticleAdapters3_posY, old_pos_y, nb_ParticleAdapters3 * sizeof(float));
        memcpy(m_ParticleAdapters3_mass, old_mass, nb_ParticleAdapters3 * sizeof(float));

        delete[] old_pos_x;
        delete[] old_pos_y;
        delete[] old_mass;

        // fill with random values between world bounds
        for(int i = 0; i < size; i++)
        {
            m_ParticleAdapters3_posX[i+nb_ParticleAdapters3] = particles[i].pos_x;
            m_ParticleAdapters3_posY[i+nb_ParticleAdapters3] = particles[i].pos_y;
            m_ParticleAdapters3_mass[i+nb_ParticleAdapters3] = particles[i].mass;
        }
        
        nb_ParticleAdapters3 += size;
    }else
    {
        // Allocate the memory for the particles
        m_ParticleAdapters3_posX = new float[size];
        m_ParticleAdapters3_posY = new float[size];
        m_ParticleAdapters3_mass = new float[size];

        // fill with random values between world bounds
        for(int i = 0; i < size; i++)
        {
            m_ParticleAdapters3_posX[i+nb_ParticleAdapters3] = particles[i].pos_x;
            m_ParticleAdapters3_posY[i+nb_ParticleAdapters3] = particles[i].pos_y;
            m_ParticleAdapters3_mass[i+nb_ParticleAdapters3] = particles[i].mass;
        }

        nb_ParticleAdapters3 = size;
    }

    OnParticlesAdded.Emit(size);
    m_ParticleAdaptersMutex.unlock();
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

    // shader_ui
    shader_ui->Use();
    // Update the model matrix
    shader_ui->SetMat4("model", m_ModelMatrix);

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
        case SHADER_BUTTON:
            return shader_button;
        case SHADER_UI:
            return shader_ui;
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

