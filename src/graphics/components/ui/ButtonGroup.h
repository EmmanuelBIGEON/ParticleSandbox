#pragma once

#include <set>

#include "Button.h"

//! \class ButtonGroup
class ButtonGroup
{
  public: 
    ButtonGroup();
    virtual ~ButtonGroup();

    void Select(Button* button);
    void AddButton(Button* button);

  private:

    std::set<Button*> m_Buttons;

};