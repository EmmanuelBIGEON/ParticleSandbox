#include "Application.h"
#include "Window.h"

#include "../graphics/main/GraphicContext.h"
#include "../graphics/components/adapters/ParticleAdapter.h"
#include "Scene.h"

Application::Application() : m_Window(nullptr), m_GraphicContext(nullptr), m_currentScene(nullptr)
{
}

Application::~Application()
{
}

void Application::Run()
{
    m_GraphicContext = new GraphicContext();
    CreateWindow(); 
    
    // Unreachable code.
    // The application loop is in the window.
    // If needed, multithread is an option.
}

void Application::CreateWindow()
{
    Window* window = new Window();

    window->OnWindowReady.Connect([this](void)
    {
        // Initialize the graphic context.
        m_GraphicContext->Init();

        // Initiliaze needed objects.
        ParticleAdapter::LoadParticleVAO();

        // Load the scene.
        // LoadScene_1();
        LoadScene_2();
        
    });

    window->Display(this);
}


void Application::Render()
{
    // m_GraphicContext->Render();
    if(m_currentScene)
    {
        m_currentScene->Update();
    }
}

void Application::LoadScene_1()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_1(m_GraphicContext);
}

void Application::LoadScene_2()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_2(m_GraphicContext);
}
