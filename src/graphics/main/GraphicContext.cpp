#include "GraphicContext.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

#include "GraphicObject.h"

#include "Shader.h"
#include "Font.h"

GraphicContext::GraphicContext() : okRendering(false), m_Objects(), needUpdate(true)
{
    glm::mat4 model = glm::mat4(1.0f);
    // translate for the center of the screen (because OpenGL is in [-1, 1])
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f/800.0f, 1.0f/600.0f, 1.0f));
    glm::mat4 projection = glm::mat4(1.0f);
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
    // shader.Use();
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
    m_Objects.push_back(object);
}


float GraphicContext::Convert_glX_to_WorldX(float x) const
{
    // [-1, 1] -> [0,1600]
    return (x + 1.0f) * 800.0f;

}

float GraphicContext::Convert_glY_to_WorldY(float y) const
{
    // [-1, 1] -> [0,1200]
    return (y + 1.0f) * 600.0f;
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
        }
        object->Draw();
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
    // Update the projection matrix
    shader_text->SetMat4("projection", m_ProjectionMatrix);
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
    shader_lighting->setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    // Update the lightColor
    shader_lighting->setVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    // Update the lightPos
    shader_lighting->setVec3("lightPos",  glm::vec3(0.0f, 0.0f, 3.0f));
    // Update the viewPos
    shader_lighting->setVec3("viewPos",  glm::vec3(0.0f, 0.0f, 3.0f));

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
    }
    return nullptr;
}