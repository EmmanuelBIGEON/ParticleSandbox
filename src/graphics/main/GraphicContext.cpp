#include "GraphicContext.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

#include "Shader.h"
#include "../adapters/TriangleAdapter.h"

GraphicContext::GraphicContext() : okRendering(false), m_Objects()
{
}

GraphicContext::~GraphicContext()
{
}

void GraphicContext::Init()
{
    TriangleAdapter* adapter = new TriangleAdapter(this, Triangle(Point(0.5, 0), Point(0.5, 0.5), Point(1, 1)));
    
    
    // SHADER_BASIC
    shader_basic = new Shader("shaders/basic.vs", "shaders/basic.fs");
    // shader.Use();

    okRendering = true;
}

void GraphicContext::Register(GraphicObject* object)
{
    m_Objects.push_back(object);
}

void GraphicContext::Render()
{
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
        }

        object->Draw();

    }
    
    // std::cout << "Rendering" << std::endl;
}

void GraphicContext::Update()
{
    // Do nothing..
}
