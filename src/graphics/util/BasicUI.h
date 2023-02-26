#pragma once

#include "../components/common/GraphicText.h"
#include "../main/GraphicContext.h"
#include "../main/GraphicObject.h"

#include "../../common/Window.h"

//! \class BasicUI
//! \brief Display the FPS and the number of particules in the context.
class BasicUI
{
    public:
        BasicUI(int startingX, int startingY, int width, int height);
        virtual ~BasicUI();

        void Init(GraphicContext* context);
        void Update();

    private:
        int m_startingX;
        int m_startingY;
        int m_width;
        int m_height;

        int m_particlesCount;
        int m_fps;

        GraphicText* m_particlesCountText;
        GraphicText* m_fpsText;

        // Create the slot here so they are destroyed on the destruction of the UI
        Slot<GraphicObject*>* m_particleAddedSlot;
        Slot<GraphicObject*>* m_particleRemovedSlot;
};