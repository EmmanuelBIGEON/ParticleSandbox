#include "SceneSwitcher.h"


SceneSwitcher::SceneSwitcher(GraphicContext* context, const glm::vec2& startingPosition, const glm::vec2& sizeSlot) : m_Context(context), 
  m_Position(startingPosition), m_sizeSlot(sizeSlot)
{
}

SceneSwitcher::~SceneSwitcher()
{
  for(const auto &elem : m_Slots)
  {
    delete (elem.second);
  }
}

void SceneSwitcher::AddSlot(SceneId id)
{
}

void SceneSwitcher::SetSlotColor(const glm::vec3& color)
{
}

void SceneSwitcher::SetSlotActiveColor(const glm::vec3& color)
{
}
