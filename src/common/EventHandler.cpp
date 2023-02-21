#include "EventHandler.h"

EventHandler* global_EventHandler = nullptr;

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

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