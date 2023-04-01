#include "EventHandler.h"

#include "../graphics/main/GraphicContext.h"
#include "../graphics/main/ParticleImpl.h"

#include "../graphics/components/adapters/CircleAdapter.h"
#include "../graphics/components/adapters/PanickedCircle.h"
#include "../graphics/components/adapters/WigglingCircle.h"
#include <glm/glm.hpp>
#include <vector>

#include "Application.h"

EventHandler* global_EventHandler = nullptr;

void EventHandler_Test::HandleEvent(const Event& event)
{
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
                // std::cout << "EventType::EVENT_MOUSE : " << mouseEvent->posX << " " << mouseEvent->posY << std::endl;
            }
            else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_PRESSED)
            {
                MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
                std::cout << "pressed : " << mouseEvent->posX << " " << mouseEvent->posY << " " << mousePressedEvent->m_Button << std::endl;
            }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
            {
                MouseReleasedEvent* mouseReleasedEvent = (MouseReleasedEvent*)&event;
                std::cout << "release : " << mouseEvent->posX << " " << mouseEvent->posY << " " << mouseReleasedEvent->m_Button << std::endl;
            }
            break;
        }
        default:
            std::cout << "EventType::EVENT_NONE" << std::endl;
            break;
    }
}


EventHandler_ParticleCreator::EventHandler_ParticleCreator(GraphicContext* context) : m_Context(context)
{
    m_SlotParticleCreator = m_Context->OnMouseReleased.Connect([this](float x, float y) {
        ParticleStruct particle;
        particle.pos_x = x;
        particle.pos_y = y;
        particle.mass = 1.0f;
        std::vector<ParticleStruct> particles;
        particles.push_back(particle);
        m_Context->AddParticles(particles);
    });
}

EventHandler_ParticleCreator::~EventHandler_ParticleCreator() 
{
    m_Context->OnMouseReleased.Disconnect(m_SlotParticleCreator);
}


//! \brief Create a particle on click
void EventHandler_ParticleCreator::HandleEvent(const Event& event)
{
    if(event.GetEventType() == EventType::EVENT_MOUSE)
    {
        MouseEvent* mouseEvent = (MouseEvent*)&event;
        if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_MOVED)
        {
            MouseMovedEvent* mousePressedEvent = (MouseMovedEvent*)&event;
            float xCenter = (float)mouseEvent->posX;
            float yCenter = (float)mouseEvent->posY; 
            xCenter = (xCenter / (float)Application::viewportWidth) * 2.0f - 1.0f;
            yCenter = (yCenter / (float)Application::viewportHeight) * 2.0f - 1.0f;
            yCenter = -yCenter;
            xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
            yCenter = m_Context->Convert_glY_to_WorldY(yCenter);

            // Detect if the mouse is over an UI object.
            // m_Context->SetMousePosition(glm::vec2(xCenter,yCenter));
            m_Context->OnMouseMoved.Emit(xCenter,yCenter);
        }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
        {
            MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
            if(mousePressedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Application::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Application::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);
                m_Context->OnMouseReleased.Emit(xCenter,yCenter); // TODO (create a particle on click despite the UI)
            }
        }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_PRESSED)
        {
            MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
            if(mousePressedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Application::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Application::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);
                m_Context->OnMousePressed.Emit(xCenter,yCenter);
            }
        }
    }else if(event.GetEventType() == EventType::EVENT_WHEEL)
    {
        WheelEvent* wheelEvent = (WheelEvent*)&event;
        // if(wheelEvent->m_OffsetY > 0)
        // {
        //     m_Context->ZoomIn(0.0f);
        // }else if(wheelEvent->m_OffsetY < 0)
        // {
        //     m_Context->ZoomOut(0.0f);
        // }

    }
}

void EventHandler_UI::HandleEvent(const Event& event)
{
    if(event.GetEventType() == EventType::EVENT_MOUSE)
    {
        MouseEvent* mouseEvent = (MouseEvent*)&event;
        if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_MOVED)
        {
            MouseMovedEvent* mousePressedEvent = (MouseMovedEvent*)&event;
            float xCenter = (float)mouseEvent->posX;
            float yCenter = (float)mouseEvent->posY; 
            xCenter = (xCenter / (float)Application::viewportWidth) * 2.0f - 1.0f;
            yCenter = (yCenter / (float)Application::viewportHeight) * 2.0f - 1.0f;
            yCenter = -yCenter;
            xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
            yCenter = m_Context->Convert_glY_to_WorldY(yCenter);
            m_Context->OnMouseMoved.Emit(xCenter,yCenter);
        
        }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_PRESSED)
        {
            MousePressedEvent* mousePressedEvent = (MousePressedEvent*)&event;
            if(mousePressedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Application::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Application::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);
                m_Context->OnMousePressed.Emit(xCenter,yCenter);
            }
        }else if(mouseEvent->GetMouseEventType() == EventMouseType::EVENT_MOUSE_RELEASED)
        {
            MouseReleasedEvent* mouseReleasedEvent = (MouseReleasedEvent*)&event;
            if(mouseReleasedEvent->m_Button == MouseButton::MOUSE_BUTTON_LEFT)
            {
                float xCenter = (float)mouseEvent->posX;
                float yCenter = (float)mouseEvent->posY; 
                // Convert to OpenGL coordinates.
                xCenter = (xCenter / (float)Application::viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)Application::viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);
                m_Context->OnMouseReleased.Emit(xCenter,yCenter);
            }
        }
    }
}