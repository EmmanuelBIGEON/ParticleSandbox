#pragma once 

#include "Event.h"

#include <iostream>

class GraphicContext;

class ParticleDetector;

enum EventHandlerType
{
    EVENT_HANDLER_TEST,
    EVENT_HANDLER_TEST2,
    EVENT_HANDLER_PARTICLE_CREATOR,
    EVENT_HANDLER_PARTICLE_HIGHLIGHTER
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

//! \class EventHandler_ParticleCreator
//! \brief Create a particle on click
class EventHandler_ParticleCreator : public EventHandler
{
    public:
        EventHandler_ParticleCreator(GraphicContext* context) : m_Context(context) {}
        virtual ~EventHandler_ParticleCreator() {}

        virtual void HandleEvent(const Event& event);

    private: 
        GraphicContext* m_Context;

};

//! \class EventHandler_ParticleHighlighter
//! \brief Highlight a particle on hovering
class EventHandler_ParticleHighlighter : public EventHandler
{
    public:
        EventHandler_ParticleHighlighter(GraphicContext* context);
        virtual ~EventHandler_ParticleHighlighter() {}

        virtual void HandleEvent(const Event& event);

    private: 
        GraphicContext* m_Context;
        ParticleDetector* m_ParticleDetector;

};



// (Be careful, for optimization purpose, it won't be tested if it's value is null)
extern EventHandler* global_EventHandler;