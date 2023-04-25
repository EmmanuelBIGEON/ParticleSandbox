#pragma once

#include "../main/GraphicContext.h"
#include "../main/GraphicObject.h"
#include "../../common/Application.h"

class Button;
class Slider;
class Checkbox;
class GraphicText;
class RectangleAdapter;

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
        Slot<int>* m_particleAddedSlot;
        // Slot<GraphicObject*>* m_particleRemovedSlot;
        RectangleAdapter* m_rectangleUI; // background of the UI
        
        GraphicText* label_repulsion;
        Slider* slider_repulsion;
        GraphicText* label_attraction;
        Slider* slider_attraction;
        GraphicText* label_repulsiondistance;
        Slider* slider_repulsiondistance;
        GraphicText* label_attractiondistance;
        Slider* slider_attractiondistance;
        GraphicText* label_particleradius;
        Slider* slider_particleradius;

        GraphicText* label_useVelocity;
        Checkbox* checkbox_velocityActivation;

        Button* button_play;
        Button* button_pause;
        
};