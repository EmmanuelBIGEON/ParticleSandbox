#pragma once 

#include "Event.h"

#include <iostream>

class GraphicContext;

enum EventHandlerType
{
    EVENT_HANDLER_TEST,
    EVENT_HANDLER_TEST2,
};

//! \class EventHandler
//! \brief Base class for all event handlers.
class EventHandler
{
    public:
        EventHandler();
        virtual ~EventHandler();

        virtual void HandleEvent(const Event& event) = 0;

        EventHandlerType GetEventHandlerType() const { return m_EventHandlerType; }

    protected:
        EventHandlerType m_EventHandlerType;
};

//! \class EventHandler_Test
//! \brief Simply display the event.
class EventHandler_Test : public EventHandler
{
    public:
        EventHandler_Test() {}
        virtual ~EventHandler_Test() {}

        virtual void HandleEvent(const Event& event);
};

//! \class EventHandler_Test2
//! \brief Create a circle on click
class EventHandler_Test2 : public EventHandler
{
    public:
        EventHandler_Test2(GraphicContext* context) : m_Context(context) {}
        virtual ~EventHandler_Test2() {}

        virtual void HandleEvent(const Event& event);

    private: 
        GraphicContext* m_Context;

};


// (Be careful, for optimization purpose, it won't be tested if it's value is null)
extern EventHandler* global_EventHandler;