#include "BehaviorUI.h"

#include "../../adapters/RectangleAdapter.h"
#include "../../common/GraphicText.h"

#include "../ParticleMatrix.h"
#include "../Slider.h"

#include <iostream>

BehaviorUI::BehaviorUI(int width, int height) : m_rectangleUI(nullptr), m_matrix(nullptr), m_width(width), m_height(height)
{
}

BehaviorUI::~BehaviorUI()
{
}

void BehaviorUI::Init(GraphicContext* context)
{
    std::cout << " INIT BEHAVIOR UI " << std::endl;
    // float height = 600.0f;
    // Rectangle r(Point(0.0f, GraphicContext::worldHeight), Point(m_width, GraphicContext::worldHeight - m_height));
    // m_rectangleUI = new RectangleAdapter(context, r);
    // m_rectangleUI->SetColor(glm::vec3(0.2f, 0.8f, 0.2f));
    // m_rectangleUI->SetOpacity(0.5f);

    // Calculate center of the UI
    float center = m_width / 2.0f;

    // Start Drawing UI from top !
    float cursorY = GraphicContext::worldHeight;
    cursorY -= 150.0f;

    m_matrix = new ParticleMatrix(glm::vec2(m_width/4.0f, cursorY));
    m_matrix->Init(context);

    cursorY -= 90.0f;
    label_particleradius = new GraphicText(context, "Particle Radius", glm::vec2(20.0f+ 8.0f, cursorY+50.0f), glm::vec2(20.0f + m_width, cursorY - 25.0f));
    slider_particleradius = new Slider(context, 0.0f, 10.0f, &particleRadius, 20.0f+ 85.0f, cursorY -10.0f);
    cursorY -= 65.0f;
    label_movementintensity = new GraphicText(context, "Movement intensity", glm::vec2(20.0f+ 8.0f, cursorY+50.0f), glm::vec2(20.0f + m_width, cursorY - 25.0f));
    slider_movementintensity = new Slider(context, 0.0f, 0.25f, &GraphicContext::movement_intensity, 20.0f+ 85.0f, cursorY -20.0f);
    

    slider_particleradius->OnValueChanged.Connect([]() {
        Particle_OPENGL::LoadParticleVAO();
    });
    
}

void BehaviorUI::Update()
{
}
