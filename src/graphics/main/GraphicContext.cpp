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

#include <thread>
#include <immintrin.h>

#ifdef __EMSCRIPTEN__
// Weird Emscripten problem... 
// Undefined values so i redefine it here. Need fix *
// i took the values from <avxintrin.h>
#define _CMP_EQ_OQ 0
#define _CMP_LT_OS 1
#define _CMP_LE_OS 2
#define _CMP_UNORD_Q 3
#define _CMP_NEQ_UQ 4
#define _CMP_NLT_US 5
#define _CMP_NLE_US 6
#define _CMP_ORD_Q 7
#define _CMP_EQ_UQ 8
#define _CMP_NGE_US 9
#define _CMP_NGT_US 10
#define _CMP_FALSE_OQ 11
#define _CMP_NEQ_OQ 12
#define _CMP_GE_OS 13
#define _CMP_GT_OS 14
#define _CMP_TRUE_UQ 15
#define _CMP_EQ_OS 16
#define _CMP_LT_OQ 17
#define _CMP_LE_OQ 18
#define _CMP_UNORD_S 19
#define _CMP_NEQ_US 20
#define _CMP_NLT_UQ 21
#define _CMP_NLE_UQ 22
#define _CMP_ORD_S 23
#define _CMP_EQ_US 24
#define _CMP_NGE_UQ  25
#define _CMP_NGT_UQ  26
#define _CMP_FALSE_OS  27
#define _CMP_NEQ_OS  28
#define _CMP_GE_OQ 29
#define _CMP_GT_OQ 30
#define _CMP_TRUE_US 31
#endif


#include "../../arch_support.h"

float GraphicContext::worldWidth = 1600.0f;
float GraphicContext::worldHeight = 1200.0f;
float GraphicContext::repulsion_factor = 1.21f;
float GraphicContext::attraction_factor = 0.381f;
float GraphicContext::repulsion_maximum_distance = 19.23f;
float GraphicContext::attraction_threshold_distance = 700.0f;
float GraphicContext::movement_intensity = 0.2f;
glm::vec3 GraphicContext::PA1_color = glm::vec3(0.31, 0.51, 1.00);
glm::vec3 GraphicContext::PA2_color = glm::vec3(1.00, 0.51, 0.31);
glm::vec3 GraphicContext::PA3_color = glm::vec3(0.31, 1.00, 0.51);
bool GraphicContext::useVelocity = false;
bool GraphicContext::behaviorDriven = false;

#define LOSE_ACCELERATION_FACTOR 0.45f

GraphicContext::GraphicContext() 
    : okRendering(false), m_Objects(), needUpdate(true), m_ModelMatrix(), m_ProjectionMatrix(), m_ViewMatrix(), 
    shader_basic(nullptr), shader_text(nullptr), shader_texture(nullptr), shader_lighting(nullptr), shader_line(nullptr), 
    shader_particle(nullptr), zoomFactor(1.0f), m_Pause(false)
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
    m_PA1_velocityX = nullptr;
    m_PA1_velocityY = nullptr;
    m_nb_PA1 = 0;

    m_PA2_posX = nullptr;
    m_PA2_posY = nullptr;
    m_PA2_mass = nullptr;
    m_PA2_velocityX = nullptr;
    m_PA2_velocityY = nullptr;
    m_nb_PA2 = 0;

    m_PA3_posX = nullptr;
    m_PA3_posY = nullptr;
    m_PA3_mass = nullptr;
    m_PA3_velocityX = nullptr;
    m_PA3_velocityY = nullptr;
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

    // Initiliaze behaviors
    InitBehaviors();

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
    
    if(!m_Pause)
    {
        if(IsAVX2Supported())
        {
            RenderParticles(PART_CLASS_1);
            RenderParticles(PART_CLASS_2);
            RenderParticles(PART_CLASS_3);
        }else if(IsAVXSupported()) 
        {
            RenderParticles_without_avx2(PART_CLASS_1);
            RenderParticles_without_avx2(PART_CLASS_2);
            RenderParticles_without_avx2(PART_CLASS_3);
        }else
        {
            RenderParticles_without_avx(PART_CLASS_1);
            RenderParticles_without_avx(PART_CLASS_2);
            RenderParticles_without_avx(PART_CLASS_3);
        }
    }else 
    {
        DrawParticles(PART_CLASS_1);
        DrawParticles(PART_CLASS_2);
        DrawParticles(PART_CLASS_3);
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
            }case SHADER_PARTICLE:
            {
                shader_particle->Use();
                if(!object->IsUpdated()) object->Update();   
                break;
            }
        }
        object->Draw();
    }
}

void GraphicContext::AddParticles(int nbParticles, ParticleClass particleClass,
    float rangex_min, float rangex_max, float rangey_min, float rangey_max)
{
    // mutex
    m_ParticleAdaptersMutex.lock();

    float* PA_PosX = nullptr;
    float* PA_PosY = nullptr;
    float* PA_Mass = nullptr;
    float* PA_VelX = nullptr;
    float* PA_VelY = nullptr;
    int PA_CurrentNB = 0;
        
    switch(particleClass)
    {
        case PART_CLASS_1:
        {
            PA_PosX = m_PA1_posX; PA_PosY = m_PA1_posY; PA_Mass = m_PA1_mass;
            PA_VelX = m_PA1_velocityX; PA_VelY = m_PA1_velocityY;
            PA_CurrentNB = m_nb_PA1;
            break;
        }
        case PART_CLASS_2:
        {
            PA_PosX = m_PA2_posX; PA_PosY = m_PA2_posY; PA_Mass = m_PA2_mass;
            PA_VelX = m_PA2_velocityX; PA_VelY = m_PA2_velocityY;
            PA_CurrentNB = m_nb_PA2;
            break;
        }
        case PART_CLASS_3:
        {
            PA_PosX = m_PA3_posX; PA_PosY = m_PA3_posY; PA_Mass = m_PA3_mass;
            PA_VelX = m_PA3_velocityX; PA_VelY = m_PA3_velocityY;
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
        float* old_vel_x = PA_VelX;
        float* old_vel_y = PA_VelY;

        PA_PosX = new float[(PA_CurrentNB) + nbParticles];
        PA_PosY = new float[(PA_CurrentNB) + nbParticles];
        PA_Mass = new float[(PA_CurrentNB) + nbParticles];
        PA_VelX = new float[(PA_CurrentNB) + nbParticles];
        PA_VelY = new float[(PA_CurrentNB) + nbParticles];

        memcpy(PA_PosX, old_pos_x, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_PosY, old_pos_y, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_Mass, old_mass, PA_CurrentNB * sizeof(float));
        memcpy(PA_VelX, old_vel_x, PA_CurrentNB * sizeof(float));
        memcpy(PA_VelY, old_vel_y, PA_CurrentNB * sizeof(float));

        delete[] old_pos_x;
        delete[] old_pos_y;
        delete[] old_mass;
        delete[] old_vel_x;
        delete[] old_vel_y;

        
        float modulox = rangex_max - rangex_min;
        float moduloy = rangey_max - rangey_min;
        // fill with random values between world bounds
        for(int i = PA_CurrentNB; i < PA_CurrentNB + nbParticles; i++)
        {
            PA_PosX[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(modulox)));
            PA_PosX[i] += rangex_min;

            PA_PosY[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(moduloy)));
            PA_PosY[i] += rangey_min;
            
            m_PA1_mass[i] = 1.0f;
            m_PA1_velocityX[i] = 0.0f;
            m_PA1_velocityY[i] = 0.0f;
        }

       PA_CurrentNB += nbParticles;
    }else
    {
        // Allocate the memory for the particles
        PA_PosX = new float[nbParticles];
        PA_PosY = new float[nbParticles];
        PA_Mass = new float[nbParticles];
        PA_VelX = new float[nbParticles];
        PA_VelY = new float[nbParticles];

        float modulox = rangex_max - rangex_min;
        float moduloy = rangey_max - rangey_min;
        // fill with random values between world bounds
        for(int i = 0; i < nbParticles; i++)
        {
            PA_PosX[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(modulox)));
            PA_PosX[i] += rangex_min;

            PA_PosY[i] = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(moduloy)));
            PA_PosY[i] += rangey_min;
            PA_Mass[i] = 1.0f;
            PA_VelX[i] = 0.0f;
            PA_VelY[i] = 0.0f;

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
            m_PA1_velocityX = PA_VelX;
            m_PA1_velocityY = PA_VelY;
            break;
        }
        case PART_CLASS_2:
        {
            m_nb_PA2 = PA_CurrentNB;
            m_PA2_posX = PA_PosX;
            m_PA2_posY = PA_PosY;
            m_PA2_mass = PA_Mass;
            m_PA2_velocityX = PA_VelX;
            m_PA2_velocityY = PA_VelY;
            break;
        }
        case PART_CLASS_3:
        {
            m_nb_PA3 = PA_CurrentNB;
            m_PA3_posX = PA_PosX;
            m_PA3_posY = PA_PosY;
            m_PA3_mass = PA_Mass;
            m_PA3_velocityX = PA_VelX;
            m_PA3_velocityY = PA_VelY;
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
    float* PA_VelX = nullptr;
    float* PA_VelY = nullptr;
    int PA_CurrentNB = 0;
    int size = particles.size();
        
    switch(particleClass)
    {
        case PART_CLASS_1:{
            PA_PosX = m_PA1_posX; PA_PosY = m_PA1_posY; PA_Mass = m_PA1_mass; PA_CurrentNB = m_nb_PA1; 
            PA_VelX = m_PA1_velocityX; PA_VelY = m_PA1_velocityY;
            break;
        }
        case PART_CLASS_2:{
            PA_PosX = m_PA2_posX; PA_PosY = m_PA2_posY; PA_Mass = m_PA2_mass; PA_CurrentNB = m_nb_PA2; 
            PA_VelX = m_PA2_velocityX; PA_VelY = m_PA2_velocityY;
            break;
        }
        case PART_CLASS_3:{
            PA_PosX = m_PA3_posX; PA_PosY = m_PA3_posY; PA_Mass = m_PA3_mass; PA_CurrentNB = m_nb_PA3; 
            PA_VelX = m_PA3_velocityX; PA_VelY = m_PA3_velocityY;
            break;
        }
    }

    if(PA_CurrentNB)
    {
        // Recreate the different arrays with the new size in memory, don't forget to delete the old ones and to save their values into the new one.
        float* old_pos_x = PA_PosX;
        float* old_pos_y = PA_PosY;
        float* old_mass = PA_Mass;
        float* old_vel_x = PA_VelX;
        float* old_vel_y = PA_VelY;

        PA_PosX = new float[(PA_CurrentNB) + size];
        PA_PosY = new float[(PA_CurrentNB) + size];
        PA_Mass = new float[(PA_CurrentNB) + size];
        PA_VelX = new float[(PA_CurrentNB) + size];
        PA_VelY = new float[(PA_CurrentNB) + size];

        memcpy(PA_PosX, old_pos_x, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_PosY, old_pos_y, (PA_CurrentNB) * sizeof(float));
        memcpy(PA_Mass, old_mass, PA_CurrentNB * sizeof(float));
        memcpy(PA_VelX, old_vel_x, PA_CurrentNB * sizeof(float));
        memcpy(PA_VelY, old_vel_y, PA_CurrentNB * sizeof(float));

        delete[] old_pos_x;
        delete[] old_pos_y;
        delete[] old_mass;
        delete[] old_vel_x;
        delete[] old_vel_y;

        // fill with random values between world bounds
        for(int i = 0; i < size; i++)
        {
            PA_PosX[i+PA_CurrentNB] = particles[i].pos_x;
            PA_PosY[i+PA_CurrentNB] = particles[i].pos_y;
            PA_Mass[i+PA_CurrentNB] = particles[i].mass;
            PA_VelX[i+PA_CurrentNB] = 0.0f;
            PA_VelY[i+PA_CurrentNB] = 0.0f;
        }

       PA_CurrentNB += size;
    }else
    {
        // Allocate the memory for the particles
        PA_PosX = new float[size];
        PA_PosY = new float[size];
        PA_Mass = new float[size];
        PA_VelX = new float[size];
        PA_VelY = new float[size];
        
        for(int i = 0; i < size; i++)
        {
            PA_PosX[i] = particles[i].pos_x;
            PA_PosY[i] = particles[i].pos_y;
            PA_Mass[i] = particles[i].mass;
            PA_VelX[i] = 0.0f;
            PA_VelY[i] = 0.0f;
        }
       PA_CurrentNB = size;
    }

    switch(particleClass)
    {
        case PART_CLASS_1:{
            m_nb_PA1 = PA_CurrentNB; m_PA1_posX = PA_PosX; m_PA1_posY = PA_PosY; m_PA1_mass = PA_Mass; 
            m_PA1_velocityX = PA_VelX; m_PA1_velocityY = PA_VelY;
            break;
        }
        case PART_CLASS_2:{
            m_nb_PA2 = PA_CurrentNB; m_PA2_posX = PA_PosX; m_PA2_posY = PA_PosY; m_PA2_mass = PA_Mass;
            m_PA2_velocityX = PA_VelX; m_PA2_velocityY = PA_VelY;
            break;
        }
        case PART_CLASS_3:{
            m_nb_PA3 = PA_CurrentNB; m_PA3_posX = PA_PosX; m_PA3_posY = PA_PosY; m_PA3_mass = PA_Mass;
            m_PA3_velocityX = PA_VelX; m_PA3_velocityY = PA_VelY;
            break;
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
#ifdef _MSC_VER
    m_ParticleAdaptersMutex.lock();
    
    shader_particle->Use();
    glBindVertexArray(Particle_OPENGL::pVAO);
    
    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    float* currentPA_mass = nullptr;
    float* currentPA_velX = nullptr;
    float* currentPA_velY = nullptr;
    int currentPA_nb = 0;
    
    // Reduce time to access memory by advancing the cursor instead of always accessing the element of array.
    // Since the size of x and y are the same, we can use the same cursor for both.
    float* targetend_cursor = nullptr; 
    float* currentend_cursor = nullptr;

    // resulting movement to apply to the particle
    float mvt_x = 0.0f;
    float mvt_y = 0.0f;

    // performance variables to reduce time acess and repetition of the same operation
    float worldWidthDividedBy2 = worldWidth / 2.0f;
    float worldHeightDividedBy2 = worldHeight / 2.0f;
    float perf_repulsion_factor = repulsion_factor;
    float perf_attraction_factor = attraction_factor;
    float perf_repulsion_maximum_distance = repulsion_maximum_distance;
    float perf_attraction_threshold_distance = attraction_threshold_distance;

    __m256 mm_zeros = _mm256_set1_ps(0.0f);
    __m256 mm_minus = _mm256_set1_ps(-1.0f);

    switch(particleClass)
    {
        case PART_CLASS_1:
        {
            shader_particle->SetVec3("particleColor", PA1_color);
            currentPA_x = m_PA1_posX; currentPA_y = m_PA1_posY;
            currentPA_mass = m_PA1_mass;
            currentPA_nb = m_nb_PA1;
            currentend_cursor = m_PA1_posX + m_nb_PA1;
            currentPA_velX = m_PA1_velocityX; currentPA_velY = m_PA1_velocityY;
            break;
        }
        case PART_CLASS_2:
        {
            shader_particle->SetVec3("particleColor", PA2_color);
            
            currentPA_x = m_PA2_posX; currentPA_y = m_PA2_posY;
            currentPA_mass = m_PA2_mass;
            currentPA_nb = m_nb_PA2;
            currentend_cursor = m_PA2_posX + m_nb_PA2;
            currentPA_velX = m_PA2_velocityX; currentPA_velY = m_PA2_velocityY;
            break;
        }
        case PART_CLASS_3:
        {
            shader_particle->SetVec3("particleColor", PA3_color);
            
            currentPA_x = m_PA3_posX; currentPA_y = m_PA3_posY;
            currentPA_mass = m_PA3_mass;
            currentPA_nb = m_nb_PA3;
            currentend_cursor = m_PA3_posX + m_nb_PA3;
            currentPA_velX = m_PA3_velocityX; currentPA_velY = m_PA3_velocityY;
            break;
        }
    }

    float* targetPA_x = nullptr;
    float* targetPA_y = nullptr;
    float* targetPA_mass = nullptr;
    int targetPA_nb = 0;


    while(currentPA_x < currentend_cursor)
    {
        // -- initialize configuration -- 
        // load scalar (current x and y)
        __m256 scalar_x = _mm256_set1_ps(*currentPA_x);
        __m256 scalar_y = _mm256_set1_ps(*currentPA_y);

        // Do each configuration considering the repetition of the world
        // Careful, for each configuration, sometimes the configuration is within worldbounds, you can't apply it.
        // find other configurations that are outside worldbounds to simulate the repetition of the world.
        // Ignoring first configuration (scalar_x, scalar_y), it will be the first calculation.
        std::vector<std::pair<__m256, __m256>> vec_scalarXY_config;
        vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalar_y));
        std::vector<std::pair<float, float>> vec_x_y_config; // used for remaining particles unable to fit into the 8 float buffer
        vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y));


        if(*currentPA_x < worldWidthDividedBy2)
        {
            if(*currentPA_y < worldHeightDividedBy2){
                // Bottom left corner.
                __m256 scalarX_left_to_right = _mm256_add_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_bottom_to_top = _mm256_add_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalarY_bottom_to_top)); // repetition of x and y on (bottom left transported to top right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y + worldHeight)); // repetition of y on (bottom transported to top)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y)); // repetition of x on (left transported to right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y + worldHeight)); // repetition of x and y on (bottom left transported to top right)
            }else{
                // Top left corner.
                __m256 scalarX_left_to_right = _mm256_add_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_top_to_bottom = _mm256_sub_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalarY_top_to_bottom)); // repetition of x and y on (top left transported to bottom right) 
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y - worldHeight)); // repetition of y on (top transported to bottom)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y)); // repetition of x on (left transported to right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y - worldHeight)); // repetition of x and y on (top left transported to bottom right)
            }
        }else{
            if(*currentPA_y < (worldHeight / 2)){
                // Bottom right corner.
                __m256 scalarX_right_to_left = _mm256_sub_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_bottom_to_top = _mm256_add_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalarY_bottom_to_top)); // repetition of x and y on (bottom right transported to top left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y + worldHeight)); // repetition of y on (bottom transported to top)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y)); // repetition of x on (right transported to left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y + worldHeight)); // repetition of x and y on (bottom right transported to top left)
            }else{
                // Top right corner.
                __m256 scalarX_right_to_left = _mm256_sub_ps(scalar_x, _mm256_set1_ps(worldWidth));
                __m256 scalarY_top_to_bottom = _mm256_sub_ps(scalar_y, _mm256_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalarY_top_to_bottom)); // repetition of x and y on (top right transported to bottom left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y - worldHeight)); // repetition of y on (top transported to bottom)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y)); // repetition of x on (right transported to left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y - worldHeight)); // repetition of x and y on (top right transported to bottom left)
            }
        } 
        int sizeConfig = vec_scalarXY_config.size();

        // Apply current velocity to current particle.
        if(useVelocity)
        {
            mvt_x = *currentPA_velX * LOSE_ACCELERATION_FACTOR;
            mvt_y = *currentPA_velY * LOSE_ACCELERATION_FACTOR; 
        }else
        {
            mvt_x = 0.0f;
            mvt_y = 0.0f;
        }

        // -- Prepare targeted data --
        for(auto particle_class : m_ParticleClasses)
        {

            if(particle_class == PART_CLASS_1)
            {
                targetPA_x = m_PA1_posX; targetPA_y = m_PA1_posY; targetPA_mass = m_PA1_mass; targetPA_nb = m_nb_PA1;
                targetend_cursor = m_PA1_posX + m_nb_PA1;
            }else if(particle_class == PART_CLASS_2)
            {
                targetPA_x = m_PA2_posX; targetPA_y = m_PA2_posY; targetPA_mass = m_PA2_mass; targetPA_nb = m_nb_PA2;
                targetend_cursor = m_PA2_posX + m_nb_PA2;
            }else if(particle_class == PART_CLASS_3)
            {
                targetPA_x = m_PA3_posX; targetPA_y = m_PA3_posY; targetPA_mass = m_PA3_mass; targetPA_nb = m_nb_PA3;
                targetend_cursor = m_PA3_posX + m_nb_PA3;
            }

            // Initiate behaviors
            if(behaviorDriven)
            {
                // Careful ! If behavior driven make sure that the behavior is defined for the current particle class. No test for performance !
                ParticleBehavior* behavior = m_ParticleBehaviors[std::make_pair(particleClass, particle_class)];
                perf_attraction_factor = behavior->attraction;
                perf_repulsion_factor = behavior->repulsion;
                perf_attraction_threshold_distance = behavior->attraction_distance;
                perf_repulsion_maximum_distance = behavior->repulsion_distance;
            }

            // prepare main variables
            __m256 mm_attraction_factor = _mm256_set1_ps(perf_attraction_factor);
            __m256 mm_repulsion_factor = _mm256_set1_ps(perf_repulsion_factor);
            __m256 mm_repulsion_maximum_distance = _mm256_set1_ps(perf_repulsion_maximum_distance);
            __m256 mm_attraction_threshold_distance = _mm256_set1_ps(perf_attraction_threshold_distance);

            // -- begin calculations --
            while(targetPA_x < targetend_cursor)
            {
                // load vector (other x and y)
                __m256 vecX = _mm256_load_ps(targetPA_x);
                __m256 vecY = _mm256_load_ps(targetPA_y);
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
                __m256 mask_sqrt = _mm256_cmp_ps(distance, mm_zeros, _CMP_GT_OQ); // prevent division by 0
                distance = _mm256_blendv_ps(mm_zeros, _mm256_sqrt_ps(distance), mask_sqrt);

                // // Calculate the direction between the current particle and all the others
                // Calculate the direction between the current particle and all the others normalisation of the vector
                // use mask to avoid division by 0
                __m256 direction_x = _mm256_blendv_ps(mm_zeros, _mm256_div_ps(sub_x, distance), mask_sqrt);
                __m256 direction_y = _mm256_blendv_ps(mm_zeros, _mm256_div_ps(sub_y, distance), mask_sqrt);

                // Calculate the movement of the current particle, if attraction or repulsion depending
                // on the distance between the current particle and the others
                // apply minimum distance
                __m256 attraction_x = _mm256_mul_ps(direction_x, mm_attraction_factor);
                __m256 attraction_y = _mm256_mul_ps(direction_y, mm_attraction_factor);

                // calculate repulsion 
                // invert direction
                direction_x = _mm256_mul_ps(direction_x, mm_minus);
                direction_y = _mm256_mul_ps(direction_y, mm_minus);
                __m256 repulsion_x = _mm256_mul_ps(direction_x, mm_repulsion_factor);
                __m256 repulsion_y = _mm256_mul_ps(direction_y, mm_repulsion_factor);

                // resulting movement mask
                __m256 mask_attraction = _mm256_cmp_ps(distance, mm_attraction_threshold_distance, _CMP_LT_OQ);
                __m256 mask_repulsion = _mm256_cmp_ps(distance, mm_repulsion_maximum_distance, _CMP_LT_OQ);
                __m256 mvt_x_tmp = _mm256_add_ps(_mm256_and_ps(mask_attraction, attraction_x), _mm256_and_ps(mask_repulsion, repulsion_x));
                __m256 mvt_y_tmp = _mm256_add_ps(_mm256_and_ps(mask_attraction, attraction_y), _mm256_and_ps(mask_repulsion, repulsion_y));

                float* mvt_x_tmp_ptr = (float*)&mvt_x_tmp;
                float* mvt_y_tmp_ptr = (float*)&mvt_y_tmp;
                for(int k = 0; k < 8; k++)
                {
                    mvt_x += *mvt_x_tmp_ptr;
                    mvt_y += *mvt_y_tmp_ptr;
                    mvt_x_tmp_ptr++;
                    mvt_y_tmp_ptr++;
                }

                targetPA_x += 8;
                targetPA_y += 8;
            } // end calculations with targeted buffer
            
            float direction_x = 0.0f;
            float direction_y = 0.0f;

            for(int j = 8*(targetPA_nb/8); j < targetPA_nb; j++)
            {
                // do the same thing than above
                // Calculate the minimal distance considering the available configurations
                // init distance, sub_x, sub_y
                float sub_x = *targetPA_x - *currentPA_x;
                float sub_y = *targetPA_y - *currentPA_y;
                float distance = sub_x*sub_x + sub_y*sub_y;

                for(int k= 0; k < sizeConfig; k++)
                {
                    float temp_sub_x = *targetPA_x - vec_x_y_config[k].first;
                    float temp_sub_y = *targetPA_y - vec_x_y_config[k].second;
                    float temp_distance = temp_sub_x*temp_sub_x + temp_sub_y*temp_sub_y;
                    if(temp_distance < distance)
                    {
                        distance = temp_distance;
                        sub_x = temp_sub_x;
                        sub_y = temp_sub_y;
                    }
                }

                distance = sqrt(distance);

                // Calculate the direction between the current particle and all the others
                // Calculate the direction between the current particle and all the others normalisation of the vector
                // use mask to avoid division by 0
                direction_x = 0.0f;
                direction_y = 0.0f;
                if(distance > 0.0f)
                {
                    direction_x = sub_x/distance;
                    direction_y = sub_y/distance;
                }
                if(distance < perf_attraction_threshold_distance)
                {
                    mvt_x += direction_x* perf_attraction_factor;
                    mvt_y += direction_y* perf_attraction_factor;
                }
                
                if(distance < perf_repulsion_maximum_distance)
                {
                    // invert direction
                    direction_x = -direction_x;
                    direction_y = -direction_y;
                    mvt_x += direction_x* perf_repulsion_factor;
                    mvt_y += direction_y* perf_repulsion_factor;
                }
                // Calculate the movement of the current particle, if attraction or repulsion depending
                
                targetPA_x++;
                targetPA_y++;
            }
        }
    
        mvt_x *= movement_intensity;
        mvt_y *= movement_intensity;

        *currentPA_x += mvt_x;
        *currentPA_y += mvt_y;

        // Update current particle velocity
        if(useVelocity)
        {
            *currentPA_velX = mvt_x * LOSE_ACCELERATION_FACTOR;
            *currentPA_velY = mvt_y * LOSE_ACCELERATION_FACTOR;
        }

        // // Check if the particle is out of the screen
        if(*currentPA_x < 0.0f)
            *currentPA_x = *currentPA_x + worldWidth;
        else if(*currentPA_x >= worldWidth)
            *currentPA_x = *currentPA_x - worldWidth;
        if(*currentPA_y < 0.0f)
            *currentPA_y = *currentPA_y + worldHeight;
        else if(*currentPA_y >= worldHeight)
            *currentPA_y = *currentPA_y - worldHeight;

        shader_particle->SetVec2("shiftPos", glm::vec2(*currentPA_x, *currentPA_y));
        glDrawArrays(GL_TRIANGLE_FAN, 0, Particle_OPENGL::pnbVertices);

        currentPA_x++;
        currentPA_y++;
        currentPA_velX++;
        currentPA_velY++;
    }

    m_ParticleAdaptersMutex.unlock();
#endif    
}

ParticleBehavior* GraphicContext::GetParticleBehavior(ParticleClass c1, ParticleClass c2)
{
    return m_ParticleBehaviors.at(std::pair<ParticleClass, ParticleClass>(c1, c2));
}
   
void GraphicContext::RenderParticles_without_avx2(ParticleClass particleClass)
{
    m_ParticleAdaptersMutex.lock();
    
    shader_particle->Use();
    glBindVertexArray(Particle_OPENGL::pVAO);
    
    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    float* currentPA_mass = nullptr;
    float* currentPA_velX = nullptr;
    float* currentPA_velY = nullptr;
    int currentPA_nb = 0;
    
    // Reduce time to access memory by advancing the cursor instead of always accessing the element of array.
    // Since the size of x and y are the same, we can use the same cursor for both.
    float* targetend_cursor = nullptr; 
    float* currentend_cursor = nullptr;

    // resulting movement to apply to the particle
    float mvt_x = 0.0f;
    float mvt_y = 0.0f;

    // performance variables to reduce time acess and repetition of the same operation
    float worldWidthDividedBy2 = worldWidth / 2.0f;
    float worldHeightDividedBy2 = worldHeight / 2.0f;
    float perf_repulsion_factor = repulsion_factor;
    float perf_attraction_factor = attraction_factor;
    float perf_repulsion_maximum_distance = repulsion_maximum_distance;
    float perf_attraction_threshold_distance = attraction_threshold_distance;


    switch(particleClass)
    {
        case PART_CLASS_1:
        {
            shader_particle->SetVec3("particleColor", PA1_color);
            currentPA_x = m_PA1_posX; currentPA_y = m_PA1_posY;
            currentPA_mass = m_PA1_mass;
            currentPA_nb = m_nb_PA1;
            currentend_cursor = m_PA1_posX + m_nb_PA1;
            break;
        }
        case PART_CLASS_2:
        {
            shader_particle->SetVec3("particleColor", PA2_color);
            
            currentPA_x = m_PA2_posX; currentPA_y = m_PA2_posY;
            currentPA_mass = m_PA2_mass;
            currentPA_nb = m_nb_PA2;
            currentend_cursor = m_PA2_posX + m_nb_PA2;
            break;
        }
        case PART_CLASS_3:
        {
            shader_particle->SetVec3("particleColor", PA3_color);
            
            currentPA_x = m_PA3_posX; currentPA_y = m_PA3_posY;
            currentPA_mass = m_PA3_mass;
            currentPA_nb = m_nb_PA3;
            currentend_cursor = m_PA3_posX + m_nb_PA3;
            break;
        }
    }

    float* targetPA_x = nullptr;
    float* targetPA_y = nullptr;
    float* targetPA_mass = nullptr;
    int targetPA_nb = 0;

    // prepare main variables
    __m128 mm_attraction_factor = _mm_set1_ps(perf_attraction_factor);
    __m128 mm_repulsion_factor = _mm_set1_ps(perf_repulsion_factor);
    __m128 mm_repulsion_maximum_distance = _mm_set1_ps(perf_repulsion_maximum_distance);
    __m128 mm_attraction_threshold_distance = _mm_set1_ps(perf_attraction_threshold_distance);
    __m128 mm_zeros = _mm_set1_ps(0.0f);
    __m128 mm_minus = _mm_set1_ps(-1.0f);

    while(currentPA_x < currentend_cursor)
    {

        
        // -- initialize configuration -- 
        // load scalar (current x and y)
        __m128 scalar_x = _mm_set1_ps(*currentPA_x);
        __m128 scalar_y = _mm_set1_ps(*currentPA_y);

        // Do each configuration considering the repetition of the world
        // Careful, for each configuration, sometimes the configuration is within worldbounds, you can't apply it.
        // find other configurations that are outside worldbounds to simulate the repetition of the world.
        // Ignoring first configuration (scalar_x, scalar_y), it will be the first calculation.
        std::vector<std::pair<__m128, __m128>> vec_scalarXY_config;
        vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalar_y));
        std::vector<std::pair<float, float>> vec_x_y_config; // used for remaining particles unable to fit into the 8 float buffer
        vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y));


        if(*currentPA_x < worldWidthDividedBy2)
        {
            if(*currentPA_y < worldHeightDividedBy2){
                // Bottom left corner.
                __m128 scalarX_left_to_right = _mm_add_ps(scalar_x, _mm_set1_ps(worldWidth));
                __m128 scalarY_bottom_to_top = _mm_add_ps(scalar_y, _mm_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalarY_bottom_to_top)); // repetition of x and y on (bottom left transported to top right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y + worldHeight)); // repetition of y on (bottom transported to top)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y)); // repetition of x on (left transported to right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y + worldHeight)); // repetition of x and y on (bottom left transported to top right)
            }else{
                // Top left corner.
                __m128 scalarX_left_to_right = _mm_add_ps(scalar_x, _mm_set1_ps(worldWidth));
                __m128 scalarY_top_to_bottom = _mm_sub_ps(scalar_y, _mm_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalar_y)); // repetition of x on (left transported to right)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_left_to_right, scalarY_top_to_bottom)); // repetition of x and y on (top left transported to bottom right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y - worldHeight)); // repetition of y on (top transported to bottom)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y)); // repetition of x on (left transported to right)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x + worldWidth, *currentPA_y - worldHeight)); // repetition of x and y on (top left transported to bottom right)
            }
        }else{
            if(*currentPA_y < (worldHeight / 2)){
                // Bottom right corner.
                __m128 scalarX_right_to_left = _mm_sub_ps(scalar_x, _mm_set1_ps(worldWidth));
                __m128 scalarY_bottom_to_top = _mm_add_ps(scalar_y, _mm_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_bottom_to_top)); // repetition of y on (bottom transported to top)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalarY_bottom_to_top)); // repetition of x and y on (bottom right transported to top left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y + worldHeight)); // repetition of y on (bottom transported to top)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y)); // repetition of x on (right transported to left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y + worldHeight)); // repetition of x and y on (bottom right transported to top left)
            }else{
                // Top right corner.
                __m128 scalarX_right_to_left = _mm_sub_ps(scalar_x, _mm_set1_ps(worldWidth));
                __m128 scalarY_top_to_bottom = _mm_sub_ps(scalar_y, _mm_set1_ps(worldHeight));
                vec_scalarXY_config.push_back(std::make_pair(scalar_x, scalarY_top_to_bottom)); // repetition of y on (top transported to bottom)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalar_y)); // repetition of x on (right transported to left)
                vec_scalarXY_config.push_back(std::make_pair(scalarX_right_to_left, scalarY_top_to_bottom)); // repetition of x and y on (top right transported to bottom left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x, *currentPA_y - worldHeight)); // repetition of y on (top transported to bottom)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y)); // repetition of x on (right transported to left)
                vec_x_y_config.push_back(std::make_pair(*currentPA_x - worldWidth, *currentPA_y - worldHeight)); // repetition of x and y on (top right transported to bottom left)
            }
        } 
        int sizeConfig = vec_scalarXY_config.size();

        // -- Prepare targeted data --
        for(auto particle_class : m_ParticleClasses)
        {

            if(particle_class == PART_CLASS_1)
            {
                targetPA_x = m_PA1_posX; targetPA_y = m_PA1_posY; targetPA_mass = m_PA1_mass; targetPA_nb = m_nb_PA1;
                targetend_cursor = m_PA1_posX + m_nb_PA1;
            }else if(particle_class == PART_CLASS_2)
            {
                targetPA_x = m_PA2_posX; targetPA_y = m_PA2_posY; targetPA_mass = m_PA2_mass; targetPA_nb = m_nb_PA2;
                targetend_cursor = m_PA2_posX + m_nb_PA2;
            }else if(particle_class == PART_CLASS_3)
            {
                targetPA_x = m_PA3_posX; targetPA_y = m_PA3_posY; targetPA_mass = m_PA3_mass; targetPA_nb = m_nb_PA3;
                targetend_cursor = m_PA3_posX + m_nb_PA3;
            }

            // Initiate behaviors
            if(behaviorDriven)
            {
                // Careful ! If behavior driven make sure that the behavior is defined for the current particle class. No test for performance !
                ParticleBehavior* behavior = m_ParticleBehaviors[std::make_pair(particleClass, particle_class)];
                perf_attraction_factor = behavior->attraction;
                perf_repulsion_factor = behavior->repulsion;
                perf_attraction_threshold_distance = behavior->attraction_distance;
                perf_repulsion_maximum_distance = behavior->repulsion_distance;
            }

            // Apply current velocity to current particle.
            if(useVelocity)
            {
                mvt_x = *currentPA_velX * LOSE_ACCELERATION_FACTOR;
                mvt_y = *currentPA_velY * LOSE_ACCELERATION_FACTOR; 
            }else
            {
                mvt_x = 0.0f;
                mvt_y = 0.0f;
            }

            // -- begin calculations --

            while(targetPA_x < targetend_cursor)
            {
                // load vector (other x and y)
                __m128 vecX = _mm_load_ps(targetPA_x);
                __m128 vecY = _mm_load_ps(targetPA_y);
                // Calculate the minimal distance considering the available configurations
                // init distance, sub_x, sub_y
                __m128 sub_x = _mm_sub_ps(vecX, scalar_x);
                __m128 sub_y = _mm_sub_ps(vecY, scalar_y);
                
                __m128 distance = _mm_add_ps(_mm_mul_ps(sub_x, sub_x), _mm_mul_ps(sub_y, sub_y));

                for(int k= 0; k < sizeConfig; k++)
                {
                    __m128 tempsub_x = _mm_sub_ps(vecX, vec_scalarXY_config[k].first);
                    __m128 tempsub_y = _mm_sub_ps(vecY, vec_scalarXY_config[k].second);
                    __m128 distance_temp = _mm_add_ps(_mm_mul_ps(tempsub_x, tempsub_x), _mm_mul_ps(tempsub_y, tempsub_y));
#ifdef __EMSCRIPTEN__
                    __m128 mask;
                    __m128 temp1 = _mm_and_ps(distance_temp, distance);
                    __m128 temp2 = _mm_cmplt_ps(distance_temp, distance);
                    mask = _mm_and_ps(temp1, temp2);
#else
                    __m128 mask = _mm_cmp_ps(distance_temp, distance, _CMP_LT_OQ);
#endif
                    distance = _mm_blendv_ps(distance, distance_temp, mask);
                    sub_x = _mm_blendv_ps(sub_x, tempsub_x, mask);
                    sub_y = _mm_blendv_ps(sub_y, tempsub_y, mask);
                }

                // sqrt
#ifdef __EMSCRIPTEN__
                    __m128 mask_sqrt;
                    { 
                        
                        __m128 temp1 = _mm_andnot_ps(distance, mm_zeros);  
                        __m128 temp2 = _mm_cmplt_ps(distance, mm_zeros);
                        mask_sqrt = _mm_and_ps(temp1, temp2);
                    }
#else
                __m128 mask_sqrt = _mm_cmp_ps(distance, mm_zeros, _CMP_GT_OQ); // prevent division by 0
#endif
                distance = _mm_blendv_ps(mm_zeros, _mm_sqrt_ps(distance), mask_sqrt);

                // // Calculate the direction between the current particle and all the others
                // Calculate the direction between the current particle and all the others normalisation of the vector
                // use mask to avoid division by 0
                __m128 direction_x = _mm_blendv_ps(mm_zeros, _mm_div_ps(sub_x, distance), mask_sqrt);
                __m128 direction_y = _mm_blendv_ps(mm_zeros, _mm_div_ps(sub_y, distance), mask_sqrt);

                // Calculate the movement of the current particle, if attraction or repulsion depending
                // on the distance between the current particle and the others
                // apply minimum distance
                __m128 attraction_x = _mm_mul_ps(direction_x, mm_attraction_factor);
                __m128 attraction_y = _mm_mul_ps(direction_y, mm_attraction_factor);

                // calculate repulsion 
                // invert direction
                direction_x = _mm_mul_ps(direction_x, mm_minus);
                direction_y = _mm_mul_ps(direction_y, mm_minus);
                __m128 repulsion_x = _mm_mul_ps(direction_x, mm_repulsion_factor);
                __m128 repulsion_y = _mm_mul_ps(direction_y, mm_repulsion_factor);

                // resulting movement mask
#ifdef __EMSCRIPTEN__
                __m128 mask_attraction;
                { 
                    
                    __m128 temp1 = _mm_and_ps(distance, mm_attraction_threshold_distance);  
                    __m128 temp2 = _mm_cmplt_ps(distance, mm_attraction_threshold_distance);
                    mask_attraction = _mm_and_ps(temp1, temp2);
                }
#else
                __m128 mask_attraction = _mm_cmp_ps(distance, mm_attraction_threshold_distance, _CMP_LT_OQ);
#endif

#ifdef __EMSCRIPTEN__
                __m128 mask_repulsion;
                { 
                    
                    __m128 temp1 = _mm_and_ps(distance, mm_repulsion_maximum_distance);  
                    __m128 temp2 = _mm_cmplt_ps(distance, mm_repulsion_maximum_distance);
                    mask_repulsion = _mm_and_ps(temp1, temp2);
                }
#else
                __m128 mask_repulsion = _mm_cmp_ps(distance, mm_repulsion_maximum_distance, _CMP_LT_OQ);
#endif
                __m128 mvt_x_tmp = _mm_add_ps(_mm_and_ps(mask_attraction, attraction_x), _mm_and_ps(mask_repulsion, repulsion_x));
                __m128 mvt_y_tmp = _mm_add_ps(_mm_and_ps(mask_attraction, attraction_y), _mm_and_ps(mask_repulsion, repulsion_y));

                float* mvt_x_tmp_ptr = (float*)&mvt_x_tmp;
                float* mvt_y_tmp_ptr = (float*)&mvt_y_tmp;
                for(int k = 0; k < 8; k++)
                {
                    mvt_x += *mvt_x_tmp_ptr;
                    mvt_y += *mvt_y_tmp_ptr;
                    mvt_x_tmp_ptr++;
                    mvt_y_tmp_ptr++;
                }

                targetPA_x += 8;
                targetPA_y += 8;
            } // end calculations with targeted buffer

            float direction_x = 0.0f;
            float direction_y = 0.0f;

            for(int j = 8*(targetPA_nb/8); j < targetPA_nb; j++)
            {
                // do the same thing than above
                // Calculate the minimal distance considering the available configurations
                // init distance, sub_x, sub_y
                float sub_x = *targetPA_x - *currentPA_x;
                float sub_y = *targetPA_y - *currentPA_y;
                float distance = sub_x*sub_x + sub_y*sub_y;

                for(int k= 0; k < sizeConfig; k++)
                {
                    float temp_sub_x = *targetPA_x - vec_x_y_config[k].first;
                    float temp_sub_y = *targetPA_y - vec_x_y_config[k].second;
                    float temp_distance = temp_sub_x*temp_sub_x + temp_sub_y*temp_sub_y;
                    if(temp_distance < distance)
                    {
                        distance = temp_distance;
                        sub_x = temp_sub_x;
                        sub_y = temp_sub_y;
                    }
                }

                distance = sqrt(distance);

                // Calculate the direction between the current particle and all the others
                // Calculate the direction between the current particle and all the others normalisation of the vector
                // use mask to avoid division by 0
                direction_x = 0.0f;
                direction_y = 0.0f;
                if(distance > 0.0f)
                {
                    direction_x = sub_x/distance;
                    direction_y = sub_y/distance;
                }
                if(distance < perf_attraction_threshold_distance)
                {
                    mvt_x += direction_x* perf_attraction_factor;
                    mvt_y += direction_y* perf_attraction_factor;
                }
                
                if(distance < perf_repulsion_maximum_distance)
                {
                    // invert direction
                    direction_x = -direction_x;
                    direction_y = -direction_y;
                    mvt_x += direction_x* perf_repulsion_factor;
                    mvt_y += direction_y* perf_repulsion_factor;
                }
                // Calculate the movement of the current particle, if attraction or repulsion depending
                
                targetPA_x++;
                targetPA_y++;
            }
        }
        
        mvt_x *= movement_intensity;
        mvt_y *= movement_intensity;

        *currentPA_x += mvt_x;
        *currentPA_y += mvt_y;

        // Update current particle velocity
        if(useVelocity)
        {
            *currentPA_velX = mvt_x * LOSE_ACCELERATION_FACTOR;
            *currentPA_velY = mvt_y * LOSE_ACCELERATION_FACTOR;
        }

        // // Check if the particle is out of the screen
        if(*currentPA_x < 0.0f)
            *currentPA_x = *currentPA_x + worldWidth;
        else if(*currentPA_x >= worldWidth)
            *currentPA_x = *currentPA_x - worldWidth;
        if(*currentPA_y < 0.0f)
            *currentPA_y = *currentPA_y + worldHeight;
        else if(*currentPA_y >= worldHeight)
            *currentPA_y = *currentPA_y - worldHeight;

        shader_particle->SetVec2("shiftPos", glm::vec2(*currentPA_x, *currentPA_y));
        glDrawArrays(GL_TRIANGLE_FAN, 0, Particle_OPENGL::pnbVertices);

        currentPA_x++;
        currentPA_y++;
        currentPA_velX++;
        currentPA_velY++;
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


#ifdef __EMSCRIPTEN__
    int start_thread1 = 0;
    int end_thread1 = nb_particles/3;
    int start_thread2 = end_thread1;
    int end_thread2 = 2*nb_particles/3;
    int start_thread3 = end_thread2;
    int end_thread3 = nb_particles;

    std::thread thread1(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread1, end_thread1);
    std::thread thread2(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread2, end_thread2);
    std::thread thread3(&GraphicContext::ComputeParticles_thread, this, particleClass, start_thread3, end_thread3);

    thread1.join();
    thread2.join();
    thread3.join();
    
#else
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
#endif
    shader_particle->Use();
    glBindVertexArray(Particle_OPENGL::pVAO);

    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    switch (particleClass)
    {
        case ParticleClass::PART_CLASS_1:
            currentPA_x = m_PA1_posX;
            currentPA_y = m_PA1_posY;
            shader_particle->SetVec3("particleColor", PA1_color);
            break;
        case ParticleClass::PART_CLASS_2:
            currentPA_x = m_PA2_posX;
            currentPA_y = m_PA2_posY;
            shader_particle->SetVec3("particleColor", PA2_color);
            break;
        case ParticleClass::PART_CLASS_3:
            currentPA_x = m_PA3_posX;
            currentPA_y = m_PA3_posY;
            shader_particle->SetVec3("particleColor", PA3_color);
            break;
    }

    // draw
    glBindVertexArray(Particle_OPENGL::pVAO);
    for(int i = 0; i < nb_particles; i++)
    {
        shader_particle->SetVec2("shiftPos", glm::vec2(currentPA_x[i], currentPA_y[i]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, Particle_OPENGL::pnbVertices);
    }


    m_ParticleAdaptersMutex.unlock();
}

void GraphicContext::ComputeParticles_thread(ParticleClass particleClass, int start, int end)
{
    
    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    float* currentPA_mass = nullptr;
    float* currentPA_velX = nullptr;
    float* currentPA_velY = nullptr;
    int currentPA_nb = 0;
    switch (particleClass)
    {
        case ParticleClass::PART_CLASS_1:
            currentPA_x = m_PA1_posX;
            currentPA_y = m_PA1_posY;
            currentPA_mass = m_PA1_mass;
            currentPA_velX = m_PA1_velocityX;
            currentPA_velY = m_PA1_velocityY;
            currentPA_nb = m_nb_PA1;
            break;
        case ParticleClass::PART_CLASS_2:
            currentPA_x = m_PA2_posX;
            currentPA_y = m_PA2_posY;
            currentPA_mass = m_PA2_mass;
            currentPA_velX = m_PA2_velocityX;
            currentPA_velY = m_PA2_velocityY;
            currentPA_nb = m_nb_PA2;
            break;
        case ParticleClass::PART_CLASS_3:
            currentPA_x = m_PA3_posX;
            currentPA_y = m_PA3_posY;
            currentPA_mass = m_PA3_mass;
            currentPA_velX = m_PA3_velocityX;
            currentPA_velY = m_PA3_velocityY;
            currentPA_nb = m_nb_PA3;
            break;
    }

    
    float* targetPA_x = nullptr;
    float* targetPA_y = nullptr;
    float* targetPA_mass = nullptr;
    int targetPA_nb = 0;

    float perf_attraction_factor = GraphicContext::attraction_factor;
    float perf_repulsion_factor = GraphicContext::repulsion_factor;
    float perf_attraction_threshold_distance = GraphicContext::attraction_threshold_distance;
    float perf_repulsion_maximum_distance = GraphicContext::repulsion_maximum_distance;

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
                XY_config.push_back(std::make_pair(left_to_right, bottom_to_top)); // repetition of x and y on (top right)
            }else{
                // Top left corner.
                XY_config.push_back(std::make_pair(xvalue, top_to_bottom)); // repetition of y on (top transported to bottom)
                XY_config.push_back(std::make_pair(left_to_right, yvalue)); // repetition of x on (right)
                XY_config.push_back(std::make_pair(left_to_right, top_to_bottom)); // repetition of x and y on (bottom right)
            }
        }else{
            if(yvalue < (worldHeight / 2)){
                // Bottom right corner.
                XY_config.push_back(std::make_pair(xvalue, bottom_to_top)); // repetition of y on (bottom)
                XY_config.push_back(std::make_pair(right_to_left, yvalue)); // repetition of x on (left)
                XY_config.push_back(std::make_pair(right_to_left, bottom_to_top)); // repetition of x and y on (top left)
            }else{
                // Top right corner.
                XY_config.push_back(std::make_pair(xvalue, top_to_bottom)); // repetition of y on (top transported to bottom)
                XY_config.push_back(std::make_pair(right_to_left, yvalue)); // repetition of x on (left)
                XY_config.push_back(std::make_pair(right_to_left, top_to_bottom)); // repetition of x and y on (bottom left)
            }
        } 
        int sizeConfig = XY_config.size();

        float mvt_x = 0.0f;
        float mvt_y = 0.0f;

        if(useVelocity)
        {
            mvt_x = currentPA_velX[i]; // velocity x
            mvt_y = currentPA_velY[i]; // velocity y
        }

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

            // Initiate behaviors
            if(behaviorDriven)
            {
                // Careful ! If behavior driven make sure that the behavior is defined for the current particle class. No test for performance !
                ParticleBehavior* behavior = m_ParticleBehaviors[std::make_pair(particleClass, particle_class)];
                perf_attraction_factor = behavior->attraction;
                perf_repulsion_factor = behavior->repulsion;
                perf_attraction_threshold_distance = behavior->attraction_distance;
                perf_repulsion_maximum_distance = behavior->repulsion_distance;
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

                if(distance == 0.0f)
                    continue;

                distance = sqrt(distance);

                // // Calculate the direction between the current particle and all the others
                float direction_x = sub_x / distance;
                float direction_y = sub_y / distance;

                // Calculate the movement of the current particle, if attraction or repulsion depending
                // on the distance between the current particle and the others
                attraction_x = 0.0f;
                attraction_y = 0.0f;
                repulsion_x = 0.0f;
                repulsion_y = 0.0f;

                if(distance < perf_attraction_threshold_distance)
                {
                    attraction_x = direction_x * perf_attraction_factor;
                    attraction_y = direction_y * perf_attraction_factor;
                    mvt_x += attraction_x;
                    mvt_y += attraction_y;
                }


                if(distance < perf_repulsion_maximum_distance)
                {
                    repulsion_x = -direction_x * perf_repulsion_factor;
                    repulsion_y = -direction_y * perf_repulsion_factor;
                    mvt_x += repulsion_x;
                    mvt_y += repulsion_y;
                }
            } // end calculations with targeted buffer
        }
        
        // Speed factor.
        mvt_x *= movement_intensity;
        mvt_y *= movement_intensity;

        currentPA_x[i] += mvt_x;
        currentPA_y[i] += mvt_y;

        // Update velocity
        if(useVelocity)
        {
            currentPA_velX[i] = mvt_x * LOSE_ACCELERATION_FACTOR;
            currentPA_velY[i] = mvt_y * LOSE_ACCELERATION_FACTOR;
        }

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

void GraphicContext::DrawParticles(ParticleClass particleClass)
{
    shader_particle->Use();
    glBindVertexArray(Particle_OPENGL::pVAO);

    float* currentPA_x = nullptr;
    float* currentPA_y = nullptr;
    int nb_particles = 0;

    if(particleClass == PART_CLASS_1)
    {
        currentPA_x = m_PA1_posX; currentPA_y = m_PA1_posY; nb_particles = m_nb_PA1;
        shader_particle->SetVec3("particleColor", PA1_color);
    }else if(particleClass == PART_CLASS_2)
    {
        currentPA_x = m_PA2_posX; currentPA_y = m_PA2_posY; nb_particles = m_nb_PA2;
            shader_particle->SetVec3("particleColor", PA2_color);
    }else if(particleClass == PART_CLASS_3)
    {
        currentPA_x = m_PA3_posX; currentPA_y = m_PA3_posY; nb_particles = m_nb_PA3;
        shader_particle->SetVec3("particleColor", PA3_color);
    }

    for(int i = 0; i < nb_particles; i++)
    {
        shader_particle->SetVec2("shiftPos", glm::vec2(currentPA_x[i], currentPA_y[i]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, Particle_OPENGL::pnbVertices);
    }
}

void GraphicContext::InitBehaviors()
{
    m_ParticleBehaviors.clear();

// float GraphicContext::repulsion_factor = 1.21f;
// float GraphicContext::attraction_factor = 0.381f;
// float GraphicContext::repulsion_maximum_distance = 19.23f;
// float GraphicContext::attraction_threshold_distance = 700.0f;

    // --------------------- Class 1 ---------------------
    // How Class1 affect itself
    ParticleBehavior* pb1_1 = new ParticleBehavior();
    pb1_1->attraction = 0.381f;
    pb1_1->repulsion = 100.21f;
    pb1_1->repulsion_distance = 10.23f;
    pb1_1->attraction_distance = 100.0f;

    // How Class 1 react to Class2
    ParticleBehavior* pb1_2 = new ParticleBehavior();
    pb1_2->attraction = 2.0f;
    pb1_2->repulsion = 2.0f;
    pb1_2->repulsion_distance = 20.0f;
    pb1_2->attraction_distance = 20.0f;

    // How Class 1 react to Class3
    ParticleBehavior* pb1_3 = new ParticleBehavior();
    pb1_3->attraction = 5.381f;
    pb1_3->repulsion = 100.21f;
    pb1_3->repulsion_distance = 10.23f;
    pb1_3->attraction_distance = 200.0f;

    // --------------------- Class 2 ---------------------
    // How Class2 affect itselfperf_attraction_factor
    ParticleBehavior* pb2_2 = new ParticleBehavior();
    pb2_2->attraction = 5.381f;
    pb2_2->repulsion = 100.0f;
    pb2_2->repulsion_distance = 10.0f;
    pb2_2->attraction_distance = 125.0f;

    // How Class 2 react to Class1
    ParticleBehavior* pb2_1 = new ParticleBehavior();
    pb2_1->attraction = 1.0f;
    pb2_1->repulsion = 70.21f;
    pb2_1->repulsion_distance = 200.0f;
    pb2_1->attraction_distance = 0.0f;

    // How Class 2 react to Class3
    ParticleBehavior* pb2_3 = new ParticleBehavior();
    pb2_3->attraction = 5.381f;
    pb2_3->repulsion = 100.21f;
    pb2_3->repulsion_distance = 10.0f;
    pb2_3->attraction_distance = 200.0f;

    // --------------------- Class 3 ---------------------
    // How Class3 affect itself
    ParticleBehavior* pb3_3 = new ParticleBehavior();
    pb3_3->attraction = 10.381f;
    pb3_3->repulsion = 100.21f;
    pb3_3->repulsion_distance = 2.0f;
    pb3_3->attraction_distance = 10.0f;

    // How Class 3 react to Class1
    ParticleBehavior* pb3_1 = new ParticleBehavior();
    pb3_1->attraction = 0.0381f;
    pb3_1->repulsion = 100.21f;
    pb3_1->repulsion_distance = 30.0f;
    pb3_1->attraction_distance = 700.0f;

    // How Class 3 react to Class2
    ParticleBehavior* pb3_2 = new ParticleBehavior();
    pb3_2->attraction = 5.381f;
    pb3_2->repulsion = 40.21f;
    pb3_2->repulsion_distance = 2.0f;
    pb3_2->attraction_distance = 150.0f;

    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_1, PART_CLASS_1)] =  pb1_1;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_1, PART_CLASS_2)] =  pb1_2;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_1, PART_CLASS_3)] =  pb1_3;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_2, PART_CLASS_2)] =  pb2_2;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_2, PART_CLASS_1)] =  pb2_1;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_2, PART_CLASS_3)] =  pb2_3;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_3, PART_CLASS_3)] =  pb3_3;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_3, PART_CLASS_1)] =  pb3_1;
    m_ParticleBehaviors[std::make_pair<ParticleClass, ParticleClass>(PART_CLASS_3, PART_CLASS_2)] =  pb3_2;
}


const glm::vec3& GraphicContext::GetColorParticle(ParticleClass particleClass)
{
    switch(particleClass)
    {
        case PART_CLASS_1:
            return PA1_color;
        case PART_CLASS_2:
            return PA2_color;
        case PART_CLASS_3:
            return PA3_color;
        default:
            return glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

void GraphicContext::RemoveParticles()
{
    m_ParticleAdaptersMutex.lock();

    delete[] m_PA1_posX;
    delete[] m_PA1_posY;
    delete[] m_PA1_mass;
    delete[] m_PA1_velocityX;
    delete[] m_PA1_velocityY;
    m_nb_PA1 = 0;

    delete[] m_PA2_posX;
    delete[] m_PA2_posY;
    delete[] m_PA2_mass;
    delete[] m_PA2_velocityX;
    delete[] m_PA2_velocityY;
    m_nb_PA2 = 0;

    delete[] m_PA3_posX;
    delete[] m_PA3_posY;
    delete[] m_PA3_mass;
    delete[] m_PA3_velocityX;
    delete[] m_PA3_velocityY;
    m_nb_PA3 = 0;
    
    m_ParticleAdaptersMutex.unlock();
}