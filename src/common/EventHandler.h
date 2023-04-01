#pragma once 

#include "Event.h"
#include "Signal.h"

#include <iostream>

class GraphicContext;

enum EventHandlerType
{
    EVENT_HANDLER_TEST,
    EVENT_HANDLER_PARTICLE_CREATOR,
    EVENT_HANDLER_UI
};

//! \class EventHandler
//! \brief Base class for all event handlers.
class EventHandler
{
    public:
        EventHandler() {}
        virtual ~EventHandler() {}
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

//! \class EventHandler_ParticleCreator
//! \brief Create a particle on click
class EventHandler_ParticleCreator : public EventHandler
{
    public:
        EventHandler_ParticleCreator(GraphicContext* context);
        virtual ~EventHandler_ParticleCreator();

        virtual void HandleEvent(const Event& event);

    private: 
        GraphicContext* m_Context;

        // Keeping it in the class allow to remove it when the class is destroyed.
        // Preventing the signal from calling a function on a deleted object.
        Slot<float, float>* m_SlotParticleCreator;

};

//! \class EventHandler_ParticleHighlighter
//! \brief Implement needed interaction with the UI
class EventHandler_UI : public EventHandler
{
    public:
        EventHandler_UI(GraphicContext* context) : m_Context(context) {}
        virtual ~EventHandler_UI() {}

        virtual void HandleEvent(const Event& event);
    private: 
        GraphicContext* m_Context;
};

// (Be careful, for optimization purpose, it won't be tested if it's value is null)
extern EventHandler* global_EventHandler;