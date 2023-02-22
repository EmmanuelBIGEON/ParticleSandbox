#include "GraphicContext.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

#include "Shader.h"
#include "../components/adapters/TriangleAdapter.h"
#include "../components/adapters/RectangleAdapter.h"
#include "../components/common/GraphicText.h"

#include "Font.h"

GraphicContext::GraphicContext() : okRendering(false), m_Objects(), needUpdate(true)
{
    glm::mat4 model = glm::mat4(1.0f);
    // translate for the center of the screen (because OpenGL is in [-1, 1])
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f/800.0f, 1.0f/600.0f, 1.0f));

    m_ModelMatrix = model;

    glm::mat4 projection = glm::mat4(1.0f);
    // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    m_ProjectionMatrix = projection;

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f)); 

    m_ViewMatrix = view;

    // test conversion 
    glm::vec3 input = glm::vec3(400.0f, 300.0f, 0.0f);
    glm::vec3 output = glm::vec3(m_ProjectionMatrix * m_ViewMatrix * m_ModelMatrix * glm::vec4(input, 1.0f));
    std::cout << "output: " << output.x << " " << output.y << " " << output.z << std::endl;

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
    // shader.Use();
    // -----------------------------

    // ---------- Fonts ----------
    font_main = new Font("fonts/arial.ttf", 48);
    // ----------------------------
    // ---------- OBJECTS ----------
    TriangleAdapter* adapter = new TriangleAdapter(this, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(this, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    GraphicText* text = new GraphicText(this, "Hello World", font_main, 400.0f, 300.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // -----------------------------
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

void GraphicContext::Render()
{
    if(needUpdate)
        Update();

    if(!okRendering)
        return;

    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
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
        }

        object->Draw();

    }
    
    // std::cout << "Rendering" << std::endl;
}

void GraphicContext::Update()
{
    std::cout << "Updating" << std::endl;
    shader_basic->Use();
    // Update the projection matrix
    shader_basic->SetMat4("projection", m_ProjectionMatrix);
    // Update the view matrix
    shader_basic->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_basic->SetMat4("model", m_ModelMatrix);

    shader_text->Use();
    // Update the projection matrix
    shader_text->SetMat4("projection", m_ProjectionMatrix);
    // Update the view matrix
    shader_text->SetMat4("view", m_ViewMatrix);
    // Update the model matrix
    shader_text->SetMat4("model", m_ModelMatrix);

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
    }
}