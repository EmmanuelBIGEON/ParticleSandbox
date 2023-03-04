#include "EventHandler.h"

#include "../graphics/main/GraphicContext.h"

#include "../graphics/components/adapters/CircleAdapter.h"
#include "../graphics/components/adapters/PanickedCircle.h"
#include "../graphics/components/adapters/WigglingCircle.h"
#include "../graphics/components/adapters/ParticleAdapter.h"
#include "../graphics/util/ParticleDetector.h"

#include <glm/glm.hpp>

#include "Window.h"

EventHandler* global_EventHandler = nullptr;

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

void EventHandler_Test::HandleEvent(const Event& event)
{
    return; // silent;
    std::cout << "EventHandler_Test::HandleEvent()" << std::endl;
    switch(event.GetEventType())
    {
        case EventType::EVENT_KEY:
        {
            KeyEvent* keyEvent = (KeyEvent*)&event;
            std::cout << "EventType::EVENT_KEY : " << keyEvent->key << std::endl;
            break;
        }
        case EventType::EVENT_MOUSE:
        {
            MouseEvent* mouseEvent = (MouseEvent*)&event;
            if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_MOVED)
            {
                std::cout << "EventType::EVENT_MOUSE : " << mouseEvent->posX << " " << mouseEvent->posY << std::endl;
            }
            else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_PRESSED)
            {
                MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
                std::cout << "EventType::EVENT_MOUSE : " << mouseEvent->posX << " " << mouseEvent->posY << " " << mousePressedEvent->m_Button << std::endl;
            }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
            {
                MouseReleasedEvent* mouseReleasedEvent = (MouseReleasedEvent*)&event;
                std::cout << "EventType::EVENT_MOUSE : " << mouseEvent->posX << " " << mouseEvent->posY << " " << mouseReleasedEvent->m_Button << std::endl;
            }
            break;
        }
        default:
            std::cout << "EventType::EVENT_NONE" << std::endl;
            break;
    }
}


//! \brief Create a circle on click
void EventHandler_Test2::HandleEvent(const Event& event)
{
    if(event.GetEventType() == EventType::EVENT_MOUSE)
    {
        MouseEvent* mouseEvent = (MouseEvent*)&event;
        if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
        {
            MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
            if(mousePressedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                std::cout << "Create a circle at " << mouseEvent->posX << " " << mouseEvent->posY << std::endl;

                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Window::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Window::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                // Convert to world coordinates.
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);

                // CircleAdapter* circle = new CircleAdapter(m_Context,Circle(Point(xCenter,yCenter), 100.0f));
                PanickedCircle* circle = new PanickedCircle(m_Context,Circle(Point(xCenter,yCenter), 40.0f));
                // WigglingCircle* circle = new WigglingCircle(m_Context,Circle(Point(xCenter,yCenter), 100.0f));
                // generate a random color
                float r = (float)rand() / (float)RAND_MAX;
                float g = (float)rand() / (float)RAND_MAX;
                float b = (float)rand() / (float)RAND_MAX;

                // Set the color of the circle.
                circle->SetColor(glm::vec3(r,g,b));
            }
        }
    }
}

//! \brief Create a particle on click
void EventHandler_ParticleCreator::HandleEvent(const Event& event)
{
    if(event.GetEventType() == EventType::EVENT_MOUSE)
    {
        MouseEvent* mouseEvent = (MouseEvent*)&event;
        if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
        {
            MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
            if(mousePressedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Window::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Window::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);
                std::cout << "Create a Particle  at " << xCenter << " " << yCenter << std::endl;

                // Creat the particle.
                Particle* particle = new Particle(glm::vec2(xCenter,yCenter));
                ParticleAdapter* particleAdapter = new ParticleAdapter(m_Context,particle);
                
            }
        }
    }else if(event.GetEventType() == EventType::EVENT_WHEEL)
    {
        WheelEvent* wheelEvent = (WheelEvent*)&event;
        if(wheelEvent->m_OffsetY > 0)
        {
            m_Context->ZoomIn(0.0f);
        }else if(wheelEvent->m_OffsetY < 0)
        {
            m_Context->ZoomOut(0.0f);
        }

    }
}

EventHandler_ParticleHighlighter::EventHandler_ParticleHighlighter(GraphicContext* context) : m_Context(context)
{
    m_ParticleDetector = new ParticleDetector(context, glm::vec2(0.0f,0.0f), 100.0f);

    // Connect signals of particleDetector to highlight the ParticleAdapters in red.
    m_ParticleDetector->OnNewParticles.Connect([this](std::vector<ParticleAdapter*> particles)
    {
        for(auto particle : particles)
        {
        std::cout << "New particles" << std::endl;
            particle->SetHighlight(true);
        }
    });

    m_ParticleDetector->OnErasedParticles.Connect([this](std::vector<ParticleAdapter*> particles)
    {
        std::cout << "Particles erased" << std::endl;
        for(auto particle : particles)
        {
            particle->SetHighlight(false);
        }
    });

}

void EventHandler_ParticleHighlighter::HandleEvent(const Event& event)
{
    if(event.GetEventType() == EventType::EVENT_MOUSE)
    {
        MouseEvent* mouseEvent = (MouseEvent*)&event;
        if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_MOVED)
        {
            MouseMovedEvent* mousePressedEvent = (MouseMovedEvent*)&event;
            float xCenter = (float)mouseEvent->posX;
            float yCenter = (float)mouseEvent->posY; 
            xCenter = (xCenter / (float)Window::viewportWidth) * 2.0f - 1.0f;
            yCenter = (yCenter / (float)Window::viewportHeight) * 2.0f - 1.0f;
            yCenter = -yCenter;
            xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
            yCenter = m_Context->Convert_glY_to_WorldY(yCenter);

            // Detect if the mouse is over a particle.
            m_ParticleDetector->SetCenter(glm::vec2(xCenter,yCenter));
            m_ParticleDetector->Update();
                
        }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
        {
            MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
            if(mousePressedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Window::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Window::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);

                // Creat the particle.
                Particle* particle = new Particle(glm::vec2(xCenter,yCenter));
                ParticleAdapter* particleAdapter = new ParticleAdapter(m_Context,particle);
                
            }
        }
    }
}