#include "BehaviorUI.h"

#include "../../adapters/RectangleAdapter.h"

#include <iostream>

BehaviorUI::BehaviorUI() : m_rectangleUI(nullptr)
{
}

BehaviorUI::~BehaviorUI()
{
}

void BehaviorUI::Init(GraphicContext* context)
{
    std::cout << " INIT BEHAVIOR UI " << std::endl;
    // float height = 600.0f;
    Rectangle r(Point(0.0f, GraphicContext::worldHeight), Point(200.0f, GraphicContext::worldHeight - 600.0f));
    m_rectangleUI = new RectangleAdapter(context, r);
    m_rectangleUI->SetColor(glm::vec3(0.2f, 0.8f, 0.2f));
    m_rectangleUI->SetOpacity(0.5f);
}

void BehaviorUI::Update()
{
}
