#include "GraphicsContext.h"

#include <glad/glad.h>

GraphicsContext::GraphicsContext()
{
}

GraphicsContext::~GraphicsContext()
{
}

void GraphicsContext::Init()
{
}

void GraphicsContext::Render()
{
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsContext::Update()
{
}
