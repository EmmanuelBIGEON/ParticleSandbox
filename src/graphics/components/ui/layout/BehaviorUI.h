#pragma once

#include "../../../main/GraphicContext.h"
#include "LayoutUI.h"

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
    
        RectangleAdapter* m_rectangleUI; // background of the UI
        ParticleMatrix* m_matrix;
        
        GraphicText* label_particleradius;
        Slider* slider_particleradius;
        GraphicText* label_movementintensity;
        Slider* slider_movementintensity;

        int m_width;
        int m_height;
};