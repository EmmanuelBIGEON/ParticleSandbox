#include "Application.h"
#include "Window.h"

#include "../graphics/main/GraphicsContext.h"

Application::Application() : m_Window(nullptr), m_GraphicsContext(nullptr)
{
}

Application::~Application()
{
}

void Application::Run()
{
    m_GraphicsContext = new GraphicsContext();
    CreateWindow();
    
    m_GraphicsContext->Init();
}

void Application::CreateWindow()
{
    Window* window = new Window();

    window->OnWindowReady.Connect([this](void)
    {
        m_GraphicsContext->Init();
    });
    
    window->Display(m_GraphicsContext);
}
