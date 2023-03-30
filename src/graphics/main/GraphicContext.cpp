#include "GraphicContext.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ParticleImpl.h"

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

#include "../../arch_support.h"


float GraphicContext::worldWidth = 1600.0f;
float GraphicContext::worldHeight = 1200.0f;
float GraphicContext::repulsion_factor = 0.1f;
float GraphicContext::attraction_factor = 0.1f;
float GraphicContext::repulsion_maximum_distance = 50.0f;
float GraphicContext::attraction_threshold_distance = 100.0f;


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
    m_PA1_posX = nullptr;
    m_PA1_posY = nullptr;
    m_PA1_mass = nullptr;
    m_nb_PA1 = 0;

    m_PA2_posX = nullptr;
    m_PA2_posY = nullptr;
    m_PA2_mass = nullptr;
    m_nb_PA2 = 0;

    m_PA3_posX = nullptr;
    m_PA3_posY = nullptr;
    m_PA3_mass = nullptr;
    m_nb_PA3 = 0;

    m_ParticleClasses.push_back(PART_CLASS_1);
    m_ParticleClasses.push_back(PART_CLASS_2);
    m_ParticleClasses.push_back(PART_CLASS_3);
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
    font_main = new Font("fonts/arial.ttf", 22);
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
    m_Objects.push_back(object);
    OnObjectRegistered.Emit(object);
}

void GraphicContext::Remove(GraphicObject* object)
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

    if(IsAVX2Supported())
    {
        RenderParticles(PART_CLASS_1);
        RenderParticles(PART_CLASS_2);
        RenderParticles(PART_CLASS_3);
    }else 
    {
        RenderParticles_without_avx(PART_CLASS_1);
        RenderParticles_without_avx(PART_CLASS_2);
        RenderParticles_without_avx(PART_CLASS_3);
    }

    for(auto object : m_Objects)
    {
        switch(object->GetShaderIndex())
        {
            case SHADER_BASIC:
            {
                shader_basic->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
            case SHADER_TEXT:
            {
                shader_text->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
            case SHADER_TEXTURE:
            {
                shader_texture->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
            case SHADER_LIGHTING:
            {
                shader_lighting->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
            case SHADER_LINE:
            {
                shader_line->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
            case SHADER_BUTTON:
            {
                shader_button->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }case SHADER_UI:
            {
                shader_ui->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
        }
        object->Draw();
    }
}

void GraphicContext::AddParticles(int nbParticles, ParticleClass particleClass)
{
    // mutex
    m_ParticleAdaptersMutex.lock();

    float* PA_PosX = nullptr;
    float* PA_PosY = nullptr;
    float* PA_Mass = nullptr;
    int PA_CurrentNB = 0;
        
    switch(particleClass)
    {
        case PART_CLASS_1:
        {
            PA_PosX = m_PA1_posX; PA_PosY = m_PA1_posY; PA_Mass = m_PA1_mass;
            PA_CurrentNB = m_nb_PA1;
            break;
        }
        case PART_CLASS_2:
        {
            PA_PosX = m_PA2_posX; PA_PosY = m_PA2_posY; PA_Mass = m_PA2_mass;
            PA_CurrentNB = m_nb_PA2;
            break;
        }
        case PART_CLASS_3:
        {
            PA_PosX = m_PA3_posX; PA_PosY = m_PA3_posY; PA_Mass = m_PA3_mass;
            PA_CurrentNB = m_nb_PA3;
            break;
        }
    }

    if(PA_CurrentNB)
    {
        // Recreate the different arrays with the new size in memory, don't forget to delete the old ones and to save their values into the new one.
        // Last size = m_nb_PA1
        float* old_pos_x = PA_PosX;
        float* old_pos_y = PA_PosY;
        float* old_mass = PA_Mass;

        PA_PosX = new float[(PA_CurrentNB) + nbParticles];
        PA_PosY = new float[(PA_CurrentNB) + nbParticles];
        PA_Mass = new float[(PA_CurrentNB) + nbParticles];

        memcpy(PA_PosX, old_pos_x, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_PosY, old_pos_y, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_Mass, old_mass, PA_CurrentNB * sizeof(float));

        delete[] old_pos_x;
        delete[] old_pos_y;
        delete[] old_mass;

        // fill with random values between world bounds
        for(int i = PA_CurrentNB; i < PA_CurrentNB + nbParticles; i++)
        {
            PA_PosX[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1600.0f)));
            PA_PosY[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1200.0f)));
            m_PA1_mass[i] = 1.0f;
        }

       PA_CurrentNB += nbParticles;
    }else
    {
        // Allocate the memory for the particles
        PA_PosX = new float[nbParticles];
        PA_PosY = new float[nbParticles];
        PA_Mass = new float[nbParticles];

        // fill with random values between world bounds
        for(int i = 0; i < nbParticles; i++)
        {
            PA_PosX[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1600.0f)));
            PA_PosY[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1200.0f)));
            PA_Mass[i] = 1.0f;
        }

       PA_CurrentNB = nbParticles;

    }

    // affect PA_CurrentNB to the right variable
    switch(particleClass)
    {
        case PART_CLASS_1:
        {
            m_nb_PA1 = PA_CurrentNB;
            m_PA1_posX = PA_PosX;
            m_PA1_posY = PA_PosY;
            m_PA1_mass = PA_Mass;
            break;
        }
        case PART_CLASS_2:
        {
            m_nb_PA2 = PA_CurrentNB;
            m_PA2_posX = PA_PosX;
            m_PA2_posY = PA_PosY;
            m_PA2_mass = PA_Mass;
            break;
        }
        case PART_CLASS_3:
        {
            m_nb_PA3 = PA_CurrentNB;
            m_PA3_posX = PA_PosX;
            m_PA3_posY = PA_PosY;
            m_PA3_mass = PA_Mass;
            break;
        }
    }

    OnParticlesAdded.Emit(nbParticles);

    m_ParticleAdaptersMutex.unlock();
}

void GraphicContext::AddParticles(const std::vector<ParticleStruct>& particles, ParticleClass particleClass)
{
        // mutex
    m_ParticleAdaptersMutex.lock();

    float* PA_PosX = nullptr;
    float* PA_PosY = nullptr;
    float* PA_Mass = nullptr;
    int PA_CurrentNB = 0;
    int size = particles.size();
        
    switch(particleClass)
    {
        case PART_CLASS_1:{
            PA_PosX = m_PA1_posX; PA_PosY = m_PA1_posY; PA_Mass = m_PA1_mass; PA_CurrentNB = m_nb_PA1; break;
        }
        case PART_CLASS_2:{
            PA_PosX = m_PA2_posX; PA_PosY = m_PA2_posY; PA_Mass = m_PA2_mass; PA_CurrentNB = m_nb_PA2; break;
        }
        case PART_CLASS_3:{
            PA_PosX = m_PA3_posX; PA_PosY = m_PA3_posY; PA_Mass = m_PA3_mass; PA_CurrentNB = m_nb_PA3; break;
        }
    }

    if(PA_CurrentNB)
    {
        // Recreate the different arrays with the new size in memory, don't forget to delete the old ones and to save their values into the new one.
        float* old_pos_x = PA_PosX;
        float* old_pos_y = PA_PosY;
        float* old_mass = PA_Mass;

        PA_PosX = new float[(PA_CurrentNB) + size];
        PA_PosY = new float[(PA_CurrentNB) + size];
        PA_Mass = new float[(PA_CurrentNB) + size];

        memcpy(PA_PosX, old_pos_x, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_PosY, old_pos_y, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_Mass, old_mass, PA_CurrentNB * sizeof(float));

        delete[] old_pos_x;
        delete[] old_pos_y;
        delete[] old_mass;

        // fill with random values between world bounds
        for(int i = 0; i < size; i++)
        {
            PA_PosX[i+PA_CurrentNB] = particles[i].pos_x;
            PA_PosY[i+PA_CurrentNB] = particles[i].pos_y;
            PA_Mass[i+PA_CurrentNB] = particles[i].mass;
        }

       PA_CurrentNB += size;
    }else
    {
        // Allocate the memory for the particles
        PA_PosX = new float[size];
        PA_PosY = new float[size];
        PA_Mass = new float[size];
        
        // fill with random values between world bounds
        for(int i = 0; i < size; i++)
        {
            PA_PosX[i] = particles[i].pos_x;
            PA_PosY[i] = particles[i].pos_y;
            PA_Mass[i] = particles[i].mass;
        }
       PA_CurrentNB = size;
    }

    switch(particleClass)
    {
        case PART_CLASS_1:{
            m_nb_PA1 = PA_CurrentNB; m_PA1_posX = PA_PosX; m_PA1_posY = PA_PosY; m_PA1_mass = PA_Mass; break;
        }
        case PART_CLASS_2:{
            m_nb_PA2 = PA_CurrentNB; m_PA2_posX = PA_PosX; m_PA2_posY = PA_PosY; m_PA2_mass = PA_Mass;break;
        }
        case PART_CLASS_3:{
            m_nb_PA3 = PA_CurrentNB; m_PA3_posX = PA_PosX; m_PA3_posY = PA_PosY; m_PA3_mass = PA_Mass;break;
        }
    }

    OnParticlesAdded.Emit(size);
    m_ParticleAdaptersMutex.unlock();
}

void GraphicContext::Update()
{
    // Shader_basic
    shader_basic->Use();
    shader_basic->SetMat4("model", m_ModelMatrix);

    // Shader_text
    shader_text->Use();
    shader_text->SetMat4("view", m_ViewMatrix);
    shader_text->SetMat4("model", m_ModelMatrix);

    // Shader_texture
    shader_texture->Use();
    shader_texture->SetMat4("projection", m_ProjectionMatrix);
    shader_texture->SetMat4("view", m_ViewMatrix);
    shader_texture->SetMat4("model", m_ModelMatrix);
    
    // Shader_lighting [won't be reshapped, kept for testing purposes]
    shader_lighting->Use();
    shader_lighting->SetMat4("projection", m_ProjectionMatrix);
    shader_lighting->SetMat4("view", m_ViewMatrix);
    shader_lighting->SetMat4("model", m_ModelMatrix);
    shader_lighting->SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    shader_lighting->SetVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    shader_lighting->SetVec3("lightPos",  glm::vec3(0.0f, 0.0f, 3.0f));
    shader_lighting->SetVec3("viewPos",  glm::vec3(0.0f, 0.0f, 3.0f));

    // shader_line
    shader_line->Use();
    shader_line->SetMat4("projection", m_ProjectionMatrix);
    shader_line->SetMat4("view", m_ViewMatrix);
    shader_line->SetMat4("model", m_ModelMatrix);

    // shader_particle
    shader_particle->Use();
    shader_particle->SetMat4("model", m_ModelMatrix);

    // shader_button
    shader_button->Use();
    shader_button->SetMat4("model", m_ModelMatrix);

    // shader_ui
    shader_ui->Use();
    shader_ui->SetMat4("model", m_ModelMatrix);

    needUpdate = false;
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

void GraphicContext::RenderParticles(ParticleClass particleClass)
{
    m_ParticleAdaptersMutex.lock();
    
    shader_particle->Use();
    glBindVertexArray(Particle_OPENGL::VAO);
    
    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    float* currentPA_mass = nullptr;
    int currentPA_nb = 0;
    switch(particleClass)
    {
        case PART_CLASS_1:
        {
            shader_particle->SetVec3("particleColor", glm::vec3(0.21, 0.41, 0.91));
            // Would normally look for behavior here, for now, behavior is static.
            currentPA_x = m_PA1_posX; currentPA_y = m_PA1_posY;
            currentPA_mass = m_PA1_mass;
            currentPA_nb = m_nb_PA1;
            break;
        }
        case PART_CLASS_2:
        {
            shader_particle->SetVec3("particleColor", glm::vec3(0.91, 0.41, 0.21));
            
            currentPA_x = m_PA2_posX; currentPA_y = m_PA2_posY;
            currentPA_mass = m_PA2_mass;
            currentPA_nb = m_nb_PA2;
            break;
        }
        case PART_CLASS_3:
        {
            shader_particle->SetVec3("particleColor", glm::vec3(0.21, 0.91, 0.41));
            
            currentPA_x = m_PA3_posX; currentPA_y = m_PA3_posY;
            currentPA_mass = m_PA3_mass;
            currentPA_nb = m_nb_PA3;
            break;
        }
    }

    float* targetPA_x = nullptr;
    float* targetPA_y = nullptr;
    float* targetPA_mass = nullptr;
    int targetPA_nb = 0;

    // prepare main variables
    __m256 mm_attraction_factor = _mm256_set1_ps(attraction_factor);
    __m256 mm_repulsion_factor = _mm256_set1_ps(repulsion_factor);
    __m256 mm_repulsion_maximum_distance = _mm256_set1_ps(repulsion_maximum_distance);
    __m256 mm_attraction_threshold_distance = _mm256_set1_ps(attraction_threshold_distance);
    
    for(int i = 0; i < currentPA_nb; i++)
    {
        
        // -- initialize configuration -- 
        // load scalar (current x and y)
        float xvalue = currentPA_x[i];
        float yvalue = currentPA_y[i];
        __m256 scalar_x = _mm256_set1_ps(xvalue);
        __m256 scalar_y = _mm256_set1_ps(yvalue);

        // Do each configuration considering the repetition of the world
        // Careful, for each configuration, sometimes the configuration is within worldbounds, you can't apply it.
        // find other configurations that are outside worldbounds to simulate the repetition of the world.
        // Ignoring first configuration (scalar_x, scalar_y), it will be the first calculation.
        std::vector<std::pair<__m256, __m256>> vec_scalarXY_config;
        vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalar_y));


        if(xvalue < (worldWidth / 2))
        {
            if(yvalue < (worldHeight / 2)){
                // Bottom left corner.
                __m256 scalarX_left_to_right = _mm256_add_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_bottom_to_top = _mm256_add_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
            }else{
                // Top left corner.
                __m256 scalarX_left_to_right = _mm256_add_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_top_to_bottom = _mm256_sub_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
            }
        }else{
            if(yvalue < (worldHeight / 2)){
                // Bottom right corner.
                __m256 scalarX_right_to_left = _mm256_sub_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_bottom_to_top = _mm256_add_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
            }else{
                // Top right corner.
                __m256 scalarX_right_to_left = _mm256_sub_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_top_to_bottom = _mm256_sub_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
            }
        } 
        int sizeConfig = vec_scalarXY_config.size();

        float mvt_x = 0.0f;
        float mvt_y = 0.0f;
        // -- Prepare targeted data --
        for(auto particle_class : m_ParticleClasses)
        {

            if(particle_class == PART_CLASS_1)
            {
                targetPA_x = m_PA1_posX; targetPA_y = m_PA1_posY; targetPA_mass = m_PA1_mass; targetPA_nb = m_nb_PA1;
            }else if(particle_class == PART_CLASS_2)
            {
                targetPA_x = m_PA2_posX; targetPA_y = m_PA2_posY; targetPA_mass = m_PA2_mass; targetPA_nb = m_nb_PA2;
            }else if(particle_class == PART_CLASS_3)
            {
                targetPA_x = m_PA3_posX; targetPA_y = m_PA3_posY; targetPA_mass = m_PA3_mass; targetPA_nb = m_nb_PA3;
            }

            // -- begin calculations --
            for(int j = 0; (j+8) <= targetPA_nb; j+=8)
            {
                // load vector (other x and y)
                __m256 vecX = _mm256_load_ps(&targetPA_x[j]);
                __m256 vecY = _mm256_load_ps(&targetPA_y[j]);
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
                __m256 mask_attraction = _mm256_cmp_ps(distance, mm_attraction_threshold_distance, _CMP_LT_OQ);
                // apply minimum distance
                __m256 attraction_x = _mm256_blendv_ps(_mm256_set1_ps(0.0f),_mm256_mul_ps(direction_x, mm_attraction_factor), mask_attraction);
                __m256 attraction_y = _mm256_blendv_ps(_mm256_set1_ps(0.0f),_mm256_mul_ps(direction_y, mm_attraction_factor), mask_attraction);

                // calculate repulsion 
                // invert direction
                __m256 mask_repulsion = _mm256_cmp_ps(distance, mm_repulsion_maximum_distance, _CMP_LT_OQ);
                direction_x = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_x, _mm256_set1_ps(-1.0f)), mask_repulsion);
                direction_y = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_y, _mm256_set1_ps(-1.0f)), mask_repulsion);
                __m256 repulsion_x = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_x, mm_repulsion_factor), mask_repulsion);
                __m256 repulsion_y = _mm256_blendv_ps(_mm256_set1_ps(0.0f), _mm256_mul_ps(direction_y, mm_repulsion_factor), mask_repulsion);

                // resulting movement mask
                __m256 mvt_x_tmp = _mm256_add_ps(attraction_x, repulsion_x);
                __m256 mvt_y_tmp = _mm256_add_ps(attraction_y, repulsion_y);

                float* mvt_x_tmp_ptr = (float*)&mvt_x_tmp;
                float* mvt_y_tmp_ptr = (float*)&mvt_y_tmp;
                for(int k = 0; k < 8; k++)
                {
                    mvt_x += mvt_x_tmp_ptr[k];
                    mvt_y += mvt_y_tmp_ptr[k];
                }
            } // end calculations with targeted buffer

            // calculate the rest of the impact of the other particles
            // will do once the formula is fully settled
            // for(int i = 8*(m_nb_PA1/8); i < m_nb_PA1; i++)
            // {
            //     // TODO
            //     // std::cout << "TODO" << std::endl;
            // }
        }
        
        currentPA_x[i] += mvt_x;
        currentPA_y[i] += mvt_y;

        // // Check if the particle is out of the screen
        if(currentPA_x[i] < 0.0f)
            currentPA_x[i] = currentPA_x[i] + 1600.0f;
        else if(currentPA_x[i] >= 1600.0f)
            currentPA_x[i] = currentPA_x[i] - 1600.0f;
        if(currentPA_y[i] < 0.0f)
            currentPA_y[i] = currentPA_y[i] + 1200.0f;
        else if(currentPA_y[i] >= 1200.0f)
            currentPA_y[i] = currentPA_y[i] - 1200.0f;

        shader_particle->SetVec2("shiftPos", glm::vec2(currentPA_x[i], currentPA_y[i]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, Particle_OPENGL::nbVertices);
    }

    m_ParticleAdaptersMutex.unlock();
    
}
   
   
void GraphicContext::RenderParticles_without_avx(ParticleClass particleClass)
{
    // std::cout << "rendering without AVX" << std::endl;
    m_ParticleAdaptersMutex.lock();

    // split particles size into 4
    int nb_particles = 0;
    switch (particleClass)
    {
        case ParticleClass::PART_CLASS_1:
            nb_particles = m_nb_PA1;
            break;
        case ParticleClass::PART_CLASS_2:
            nb_particles = m_nb_PA2;
            break;
        case ParticleClass::PART_CLASS_3:
            nb_particles = m_nb_PA3;
            break;
    }

    int start_thread1 = 0;
    int end_thread1 = nb_particles/4;
    int start_thread2 = end_thread1;
    int end_thread2 = 2*nb_particles/4;
    int start_thread3 = end_thread2;
    int end_thread3 = 3*nb_particles/4;
    int start_thread4 = end_thread3;
    int end_thread4 = nb_particles;

    std::thread thread1(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread1, end_thread1);
    std::thread thread2(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread2, end_thread2);
    std::thread thread3(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread3, end_thread3);
    std::thread thread4(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread4, end_thread4);

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    // draw
    shader_particle->Use();

    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    switch (particleClass)
    {
        case ParticleClass::PART_CLASS_1:
            currentPA_x = m_PA1_posX;
            currentPA_y = m_PA1_posY;
            shader_particle->SetVec3("particleColor", glm::vec3(0.21, 0.41, 0.91));
            break;
        case ParticleClass::PART_CLASS_2:
            currentPA_x = m_PA2_posX;
            currentPA_y = m_PA2_posY;
            shader_particle->SetVec3("particleColor", glm::vec3(0.91, 0.41, 0.21));
            break;
        case ParticleClass::PART_CLASS_3:
            currentPA_x = m_PA3_posX;
            currentPA_y = m_PA3_posY;
            shader_particle->SetVec3("particleColor", glm::vec3(0.21, 0.91, 0.41));
            break;
    }

    // draw
    glBindVertexArray(Particle_OPENGL::VAO);
    for(int i = 0; i < nb_particles; i++)
    {
        shader_particle->SetVec2("shiftPos", glm::vec2(currentPA_x[i], currentPA_y[i]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, Particle_OPENGL::nbVertices);
    }


    m_ParticleAdaptersMutex.unlock();
}


void GraphicContext::ComputeParticles_thread(ParticleClass particleClass, int start, int end)
{
    shader_particle->Use();
    glBindVertexArray(Particle_OPENGL::VAO);
    
    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    float* currentPA_mass = nullptr;
    int currentPA_nb = 0;
    switch (particleClass)
    {
        case ParticleClass::PART_CLASS_1:
            currentPA_x = m_PA1_posX;
            currentPA_y = m_PA1_posY;
            currentPA_mass = m_PA1_mass;
            currentPA_nb = m_nb_PA1;
            break;
        case ParticleClass::PART_CLASS_2:
            currentPA_x = m_PA2_posX;
            currentPA_y = m_PA2_posY;
            currentPA_mass = m_PA2_mass;
            currentPA_nb = m_nb_PA2;
            break;
        case ParticleClass::PART_CLASS_3:
            currentPA_x = m_PA3_posX;
            currentPA_y = m_PA3_posY;
            currentPA_mass = m_PA3_mass;
            currentPA_nb = m_nb_PA3;
            break;
    }

    
    float* targetPA_x = nullptr;
    float* targetPA_y = nullptr;
    float* targetPA_mass = nullptr;
    int targetPA_nb = 0;

    for(int i = start; i< end; i++)
    {
        // -- initialize configuration -- 
        // load scalar (current x and y)
        float xvalue = currentPA_x[i];
        float yvalue = currentPA_y[i];

        std::vector<std::pair<float, float>> XY_config;
        XY_config.push_back(std::make_pair(xvalue, yvalue));

        float left_to_right = xvalue + worldWidth;
        float right_to_left = xvalue - worldWidth;
        float bottom_to_top = yvalue + worldHeight;
        float top_to_bottom = yvalue - worldHeight;

        if(xvalue < (worldWidth / 2))
        {
            if(yvalue < (worldHeight / 2)){
                // Bottom left corner.
                XY_config.push_back(std::make_pair(xvalue, bottom_to_top)); // repetition of y on (top)
                XY_config.push_back(std::make_pair(left_to_right, yvalue)); // repetition of x on (right)
            }else{
                // Top left corner.
                XY_config.push_back(std::make_pair(xvalue, top_to_bottom)); // repetition of y on (top transported to bottom)
                XY_config.push_back(std::make_pair(left_to_right, yvalue)); // repetition of x on (right)
            }
        }else{
            if(yvalue < (worldHeight / 2)){
                // Bottom right corner.
                XY_config.push_back(std::make_pair(xvalue, bottom_to_top)); // repetition of y on (bottom)
                XY_config.push_back(std::make_pair(right_to_left, yvalue)); // repetition of x on (left)
            }else{
                // Top right corner.
                XY_config.push_back(std::make_pair(xvalue, top_to_bottom)); // repetition of y on (top transported to bottom)
                XY_config.push_back(std::make_pair(right_to_left, yvalue)); // repetition of x on (left)
            }
        } 
        int sizeConfig = XY_config.size();

        float mvt_x = 0.0f;
        float mvt_y = 0.0f;
        float attraction_x = 0.0f; 
        float attraction_y = 0.0f; 
        float repulsion_x = 0.0f;
        float repulsion_y = 0.0f;

        // -- Prepare targeted data --
        for(auto particle_class : m_ParticleClasses)
        {

            if(particle_class == PART_CLASS_1)
            {
                targetPA_x = m_PA1_posX; targetPA_y = m_PA1_posY; targetPA_mass = m_PA1_mass; targetPA_nb = m_nb_PA1;
            }else if(particle_class == PART_CLASS_2)
            {
                targetPA_x = m_PA2_posX; targetPA_y = m_PA2_posY; targetPA_mass = m_PA2_mass; targetPA_nb = m_nb_PA2;
            }else if(particle_class == PART_CLASS_3)
            {
                targetPA_x = m_PA3_posX; targetPA_y = m_PA3_posY; targetPA_mass = m_PA3_mass; targetPA_nb = m_nb_PA3;
            }

            // -- begin calculations --
            for(int j = 0; j< targetPA_nb; j++)
            {

                float targetX = targetPA_x[j];
                float targetY = targetPA_y[j];

                float sub_x = targetX - xvalue;
                float sub_y = targetY - yvalue;
                
                float distance = (sub_x * sub_x) + (sub_y * sub_y);

                for(int k= 0; k < sizeConfig; k++)
                {
                    float tempsub_x = targetX - XY_config[k].first;
                    float tempsub_y = targetY - XY_config[k].second;
                    float distance_temp = (tempsub_x * tempsub_x) + (tempsub_y * tempsub_y);
                    if(distance_temp < distance)
                    {
                        distance = distance_temp;
                        sub_x = tempsub_x;
                        sub_y = tempsub_y;
                    }
                }

                distance = sqrt(distance);

                // // Calculate the direction between the current particle and all the others
                float direction_x = (distance > 0.0f) ? sub_x / distance : 0.0f;
                float direction_y = (distance > 0.0f) ? sub_y / distance : 0.0f;

                // Calculate the movement of the current particle, if attraction or repulsion depending
                // on the distance between the current particle and the others
                attraction_x = 0.0f;
                attraction_y = 0.0f;
                repulsion_x = 0.0f;
                repulsion_y = 0.0f;

                if(distance > attraction_threshold_distance)
                {
                    attraction_x = (attraction_factor > 0.0f) ? direction_x * attraction_factor : 0.0f;
                    attraction_y = (attraction_factor > 0.0f) ? direction_y * attraction_factor : 0.0f;
                }


                if(distance < repulsion_maximum_distance)
                {
                    repulsion_x = (repulsion_factor > 0.0f) ? direction_x * -repulsion_factor : 0.0f;
                    repulsion_y = (repulsion_factor > 0.0f) ? direction_y * -repulsion_factor : 0.0f;
                }

                // // resulting movement mask
                float mvt_x_tmp = attraction_x + repulsion_x;
                float mvt_y_tmp = attraction_y + repulsion_y;
                mvt_x += mvt_x_tmp;
                mvt_y += mvt_y_tmp;

            } // end calculations with targeted buffer
        }
        
        currentPA_x[i] += mvt_x;
        currentPA_y[i] += mvt_y;

        // // Check if the particle is out of the screen
        if(currentPA_x[i] < 0.0f)
            currentPA_x[i] = currentPA_x[i] + 1600.0f;
        else if(currentPA_x[i] >= 1600.0f)
            currentPA_x[i] = currentPA_x[i] - 1600.0f;
        if(currentPA_y[i] < 0.0f)
            currentPA_y[i] = currentPA_y[i] + 1200.0f;
        else if(currentPA_y[i] >= 1200.0f)
            currentPA_y[i] = currentPA_y[i] - 1200.0f;
    }
}