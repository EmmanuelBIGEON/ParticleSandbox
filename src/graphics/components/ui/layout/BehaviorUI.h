#pragma once

#include "../../../main/GraphicContext.h"
#include "LayoutUI.h"

#include "../../../../common/Signal.h"

class RectangleAdapter;
class ParticleMatrix;
class Slider;
class GraphicText;
class Button;

//! \class BehaviorUI
//! \brief This class is used to draw the UI for the behavior scene.
// Allow to customize behaviors between particles.
class BehaviorUI : public LayoutUI
{
    public: 
        BehaviorUI(int width, int height);
        ~BehaviorUI();

        void ToggleDisplayed(bool displayed);

        virtual void Init(GraphicContext* context) override;
        virtual void Update() override;
    private:

        RectangleAdapter* m_rectangleUI; // background of the UI
        ParticleMatrix* m_matrix;
        
        GraphicText* label_particleradius;
        Slider* slider_particleradius;
        GraphicText* label_movementintensity;
        Slider* slider_movementintensity;

        Button* button_play;
        Button* button_pause;
        Button* button_hideui;

        // Create the slot here so they are destroyed on the destruction of the UI
        Slot<int>* m_particleAddedSlot;

        int m_width;
        int m_height;

        bool m_displayed;
        
};