#include "ButtonGroup.h"

ButtonGroup::ButtonGroup()
{
}

ButtonGroup::~ButtonGroup()
{
}

void ButtonGroup::Select(Button* button)
{
  for(const auto& elem : m_Buttons)
  {
    if(elem == button)
    {
      elem->SetActive(true);
    }else
    {
      elem->SetActive(false);
    }
  }
}

void ButtonGroup::AddButton(Button* button)
{
  m_Buttons.insert(button);
}