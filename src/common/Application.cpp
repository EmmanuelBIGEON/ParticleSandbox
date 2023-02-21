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
}

void Application::CreateWindow()
{
    Window* window = new Window();
    window->Display(m_GraphicsContext);
}
