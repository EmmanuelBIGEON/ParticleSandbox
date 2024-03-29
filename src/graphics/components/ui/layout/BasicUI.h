#pragma once

#include "../../../main/GraphicContext.h"
#include "../../../main/GraphicObject.h"
#include "../../../../common/Application.h"
#include "LayoutUI.h"

class Button;
class Slider;
class Checkbox;
class GraphicText;
class RectangleAdapter;

//! \class BasicUI
//! \brief Display the FPS and the number of particules in the context.
class BasicUI : public LayoutUI
{
    public:
        BasicUI(int startingX, int startingY, int width, int height);
        virtual ~BasicUI();

        virtual void Init(GraphicContext* context) override;
        virtual void Update() override;

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
        GraphicText* label_force;
        Slider* slider_force;
        GraphicText* label_repulsiondistance;
        Slider* slider_repulsiondistance;
        GraphicText* label_forcedistance;
        Slider* slider_forcedistance;
        GraphicText* label_particleradius;
        Slider* slider_particleradius;
        GraphicText* label_movementintensity;
        Slider* slider_movementintensity;

        GraphicText* label_useVelocity;
        Checkbox* checkbox_velocityActivation;

        Button* button_play;
        Button* button_pause;
        
};