#pragma once 

#include "Event.h"

#include <iostream>

//! \class EventHandler
//! \brief Base class for all event handlers.
class EventHandler
{
    public:
        EventHandler();
        virtual ~EventHandler();

        virtual void HandleEvent(const Event& event) = 0;
};

class EventHandler_Test : public EventHandler
{
    public:
        EventHandler_Test() {}
        virtual ~EventHandler_Test() {}

        virtual void HandleEvent(const Event& event);
};

// (Be careful, for optimization purpose, it won't be tested if it's value is null)
extern EventHandler* global_EventHandler;