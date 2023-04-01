#include "Application.h"
#include "Window.h"

#include "../graphics/main/GraphicContext.h"
#include "../graphics/main/ParticleImpl.h"
#include "Scene.h"


#ifdef __EMSCRIPTEN__
#include "../wasm/Wasm_View.h"
#endif

// lock
#include <mutex>

#include <iostream>

// singleton 
Application* Application::instance = nullptr;

std::mutex m_mutex;

int Application::viewportWidth = 1200, Application::viewportHeight = 900;
int Application::fps = 0;


Application::Application() : m_GraphicContext(nullptr), m_currentScene(nullptr)
{
#ifdef __EMSCRIPTEN__
    m_WasmView = nullptr;
#else
    m_Window = nullptr;
#endif

}

Application::~Application()
{
}

void Application::Run()
{
    m_GraphicContext = new GraphicContext();

#ifdef __EMSCRIPTEN__
    CreateWasmView();
#else
    CreateWindow(); 
    // Unreachable code.
    // The application loop is in the window.
    // If needed, multithread is an option.
#endif
}
    

#ifdef __EMSCRIPTEN__


void Application::CreateWasmView()
{
    m_WasmView = new Wasm_View();
    m_GraphicContext->Init();
    Particle_OPENGL::LoadParticleVAO();

    // Load the scene.
    LoadScene_2();
}


#else
void Application::CreateWindow()
{
    m_Window = new Window();

    m_Window->OnWindowReady.Connect([this](void)
    {
        // Initialize the graphic context.
        m_GraphicContext->Init();

        // Initiliaze needed objects.
        Particle_OPENGL::LoadParticleVAO();

        // Load the scene.
        LoadScene_Main();
        
    });

    m_Window->Display(this);
}
#endif

void Application::Render()
{
    // lock
    m_mutex.lock();
    // m_GraphicContext->Render();
    if(m_currentScene)
    {
        m_currentScene->Update();
    }
    // unlock
    m_mutex.unlock();
}

void Application::LoadScene_1()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_1(m_GraphicContext);
}

void Application::LoadScene_Main()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_Main(m_GraphicContext);
}

void Application::LoadScene_Testing()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_Testing(m_GraphicContext);
}

void Application::LoadScene_3()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_3(m_GraphicContext);
}

void Application::LoadScene_Text()
{
    if(m_currentScene)
        delete(m_currentScene);

    m_currentScene = Scene::CreateScene_Text(m_GraphicContext);
}
