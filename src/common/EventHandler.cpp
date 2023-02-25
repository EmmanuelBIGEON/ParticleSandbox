#include "EventHandler.h"

#include "../graphics/main/GraphicContext.h"

#include "../graphics/components/adapters/CircleAdapter.h"
#include "../graphics/components/adapters/PanickedCircle.h"
#include "../graphics/components/adapters/WigglingCircle.h"

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
                xCenter = (xCenter / (float)viewportWidth) * 2.0f - 1.0f;
                yCenter = (yCenter / (float)viewportHeight) * 2.0f - 1.0f;
                yCenter = -yCenter;

                std::cout << "Create a circle (GL value) at " << xCenter << " " << yCenter << std::endl;
                // reverse

                // Convert to world coordinates.
                xCenter = m_Context->Convert_glX_to_WorldX(xCenter);
                yCenter = m_Context->Convert_glY_to_WorldY(yCenter);

                // Create the circle.
                // CircleAdapter* circle = new CircleAdapter(m_Context,Circle(Point(xCenter,yCenter), 100.0f));
                // PanickedCircle* circle = new PanickedCircle(m_Context,Circle(Point(xCenter,yCenter), 100.0f));
                WigglingCircle* circle = new WigglingCircle(m_Context,Circle(Point(xCenter,yCenter), 100.0f));
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