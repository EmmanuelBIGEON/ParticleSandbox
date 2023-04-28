#pragma once

#include "../../../main/GraphicContext.h"
#include "LayoutUI.h"

class RectangleAdapter;

//! \class BehaviorUI
//! \brief This class is used to draw the UI for the behavior scene.
// Allow to customize behaviors between particles.
class BehaviorUI : public LayoutUI
{
    public: 
        BehaviorUI();
        ~BehaviorUI();

        virtual void Init(GraphicContext* context) override;
        virtual void Update() override;
    private:
    
        RectangleAdapter* m_rectangleUI; // background of the UI
};