#include "Application.h"
#include "Window.h"

#include "../graphics/main/GraphicContext.h"

Application::Application() : m_Window(nullptr), m_GraphicContext(nullptr)
{
}

Application::~Application()
{
}

void Application::Run()
{
    m_GraphicContext = new GraphicContext();
    CreateWindow();
    
    m_GraphicContext->Init();
}

void Application::CreateWindow()
{
    Window* window = new Window();

    window->OnWindowReady.Connect([this](void)
    {
        m_GraphicContext->Init();
    });

    window->Display(m_GraphicContext);
}
