#include "Application.h"
#include "Window.h"

Application::Application() : m_Window(nullptr)
{
}

Application::~Application()
{
}

void Application::Run()
{
    CreateWindow();
}

void Application::CreateWindow()
{
    Window* window = new Window();
    window->Display();
}
