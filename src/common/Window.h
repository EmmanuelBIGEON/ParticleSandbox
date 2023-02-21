#pragma once

#include "EventHandler.h"
#include "Signal.h"

#include "../graphics/main/GraphicsContext.h"

class Window
{
    public:
        Window();
        virtual ~Window();

        //! \brief Display the window.
        //! Render the graphics context.
        void Display(GraphicsContext* graphicsContext);

        
        Signal<> OnWindowReady;
};
