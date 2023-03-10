#include "Window.h"

#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Event.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

double lastMouseX = 0, lastMouseY = 0;
Window::Window()
{
    // Setup the event handler (even if it is replaced, because it is assumed that it always exists)
    global_EventHandler = new EventHandler_Test();
}

Window::~Window()
{
}

void Window::Display(Application* app)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(Application::viewportWidth, Application::viewportHeight, "ParticleSandbox", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    // callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // --------------------- OpenGL settings ---------------------
    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // enable anti-aliasing (MSAA)
    glEnable(GL_MULTISAMPLE);  

    // -----------------------------------------------------------

    OnWindowReady.Emit();

    // Apparently already capped at 60 fps
    // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw
    // For now 60 fps is okay.

    // lets count fps
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while (!glfwWindowShouldClose(window))
    {

        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) 
        {
            lastTime += 1.0;
            Application::fps = nbFrames;
            nbFrames = 0;
        }

        app->Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}


static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Application::viewportWidth = width;
    Application::viewportHeight = height;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // On cr???? l'??v??nement
    // keypressed
    if(action == GLFW_PRESS)
    {
        KeyPressedEvent event(key);
        global_EventHandler->HandleEvent(event);
    }else if(action == GLFW_RELEASE)
    {
        KeyReleasedEvent event(key);
        global_EventHandler->HandleEvent(event);
    }
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    MouseMovedEvent event(xpos, ypos);
    global_EventHandler->HandleEvent(event);   

    lastMouseX = xpos;
    lastMouseY = ypos;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        if(button == GLFW_MOUSE_BUTTON_LEFT)
        {
            MousePressedEvent event(lastMouseX, lastMouseY, MouseButton::MOUSE_BUTTON_LEFT);
            global_EventHandler->HandleEvent(event);
        }else if(button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            MousePressedEvent event(lastMouseX, lastMouseY, MouseButton::MOUSE_BUTTON_RIGHT);
            global_EventHandler->HandleEvent(event);
        }else if(button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            MousePressedEvent event(lastMouseX, lastMouseY, MouseButton::MOUSE_BUTTON_MIDDLE);
            global_EventHandler->HandleEvent(event);
        }
    
    }else if(action == GLFW_RELEASE)
    {
        if(button == GLFW_MOUSE_BUTTON_LEFT)
        {
            MouseReleasedEvent event(lastMouseX, lastMouseY, MouseButton::MOUSE_BUTTON_LEFT);
            global_EventHandler->HandleEvent(event);
        }else if(button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            MouseReleasedEvent event(lastMouseX, lastMouseY, MouseButton::MOUSE_BUTTON_RIGHT);
            global_EventHandler->HandleEvent(event);
        }else if(button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            MouseReleasedEvent event(lastMouseX, lastMouseY, MouseButton::MOUSE_BUTTON_MIDDLE);
            global_EventHandler->HandleEvent(event);
        }
    }

}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    WheelEvent event(xoffset, yoffset);
    global_EventHandler->HandleEvent(event);
}