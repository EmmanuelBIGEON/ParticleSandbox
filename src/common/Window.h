#pragma once

#include "EventHandler.h"
#include "Signal.h"

#include "../graphics/main/GraphicContext.h"

class Application;

extern int viewportWidth;
extern int viewportHeight;

class Window
{
    public:
        Window();
        virtual ~Window();

        //! \brief Display the window.
        //! Render the graphics context.
        void Display(Application* app);

        
        Signal<> OnWindowReady;
};
