#pragma once

#include <map>
#include <string>

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"
#include "../../../common/Scene.h"

#include <glm/ext.hpp>

//! \class SceneSwitcher
//! \brief Allow to switch dynamically between scenes.
class SceneSwitcher
{
  public:
    SceneSwitcher(GraphicContext* context, const glm::vec2& startingPosition, const glm::vec2& sizeSlot); 
    virtual ~SceneSwitcher();

    void AddSlot(SceneId id);

    void SetSlotColor(const glm::vec3& color);
    void SetSlotActiveColor(const glm::vec3& color);

  private:
    std::map<SceneId, Button*> m_Slots;
    GraphicContext* m_Context;

    glm::vec2 m_Position;
    glm::vec3 m_currentColor;
    glm::vec3 m_currentActiveColor;
    glm::vec2 m_sizeSlot;

    SceneId m_SelectedSlot;
};