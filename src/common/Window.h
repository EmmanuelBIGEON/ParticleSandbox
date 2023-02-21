#pragma once

#include "EventHandler.h"

class Window
{
    public:
        Window();
        virtual ~Window();

        void Display();

    protected:
        EventHandler* m_EventHandler;
};
