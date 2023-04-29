#pragma once

#include "../../../main/GraphicContext.h"
#include "LayoutUI.h"

#include "../../../../common/Signal.h"

class RectangleAdapter;
class ParticleMatrix;
class Slider;
class GraphicText;

//! \class BehaviorUI
//! \brief This class is used to draw the UI for the behavior scene.
// Allow to customize behaviors between particles.
class BehaviorUI : public LayoutUI
{
    public: 
        BehaviorUI(int width, int height);
        ~BehaviorUI();

        virtual void Init(GraphicContext* context) override;
        virtual void Update() override;
    private:
    
        GraphicText* m_particlesCountText;
        GraphicText* m_fpsText;

        RectangleAdapter* m_rectangleUI; // background of the UI
        ParticleMatrix* m_matrix;
        
        GraphicText* label_particleradius;
        Slider* slider_particleradius;
        GraphicText* label_movementintensity;
        Slider* slider_movementintensity;

        // Create the slot here so they are destroyed on the destruction of the UI
        Slot<int>* m_particleAddedSlot;

        int m_width;
        int m_height;
        
        int m_particlesCount;
        int m_fps;
};