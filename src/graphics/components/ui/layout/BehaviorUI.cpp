#include "BehaviorUI.h"

#include "../../adapters/RectangleAdapter.h"
#include "../../common/GraphicText.h"

#include "../ParticleMatrix.h"
#include "../Slider.h"
#include "../Button.h"

#include "../../../../common/Application.h"

#include <iostream>

BehaviorUI::BehaviorUI(int width, int height) : m_rectangleUI(nullptr), m_matrix(nullptr), m_width(width), m_height(height),
    label_particleradius(nullptr), slider_particleradius(nullptr), label_movementintensity(nullptr), slider_movementintensity(nullptr),
    button_hideui(nullptr), button_play(nullptr), button_pause(nullptr), m_displayed(true)
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

    cursorY -= 30.0f;
    
    button_play = new Button(context, glm::vec2(0.0f, cursorY), glm::vec2(m_width/2, 30));
    button_play->SetPathIcon("data/img/play.png");
    button_play->SetInvertIconOnActive(true);
    button_play->SetActiveColor(glm::vec3(0.0f, 0.2f, 0.71f));

    button_pause = new Button(context, glm::vec2(0.0f + m_width/2, cursorY), glm::vec2(m_width/2, 30));
    button_pause->SetPathIcon("data/img/stop.png");
    button_pause->SetInvertIconOnActive(true);
    button_pause->SetActiveColor(glm::vec3(0.0f, 0.2f, 0.71f));
    
    button_hideui = new Button(context, glm::vec2(0.0f, 0.0f), glm::vec2(150, 50));
    button_hideui->SetText("Toggle UI");
    button_hideui->SetRangeButton(true);
    button_hideui->SetRangeButtonDistance(200.0f);

    

    
    button_play->SetActive(true); // guess that the application is running by default. Might be wrong.

    cursorY -= 35.0f;
    label_particleradius = new GraphicText(context, "Particle Radius", glm::vec2(7.0f, cursorY+50.0f), glm::vec2(20.0f + m_width, cursorY - 25.0f));
    label_particleradius->SetAlignment(TextAlign::ALIGN_LEFT);
    slider_particleradius = new Slider(context, 0.0f, 10.0f, &particleRadius, 7.0f+ 73.0f, cursorY -10.0f);
    cursorY -= 65.0f;
    label_movementintensity = new GraphicText(context, "Movement intensity", glm::vec2(7.0f, cursorY+50.0f), glm::vec2(20.0f + m_width, cursorY - 25.0f));
    label_movementintensity->SetAlignment(TextAlign::ALIGN_LEFT);
    slider_movementintensity = new Slider(context, 0.0f, 0.25f, &GraphicContext::movement_intensity, 7.0f+ 73.0f, cursorY -20.0f);

    cursorY -= 160.0f;
    m_matrix = new ParticleMatrix(glm::vec2(m_width/4.0f, cursorY));
    m_matrix->Init(context);

    

    slider_particleradius->OnValueChanged.Connect([]() {
        Particle_OPENGL::LoadParticleVAO();
    });

    
    // connect play and stop
    button_play->OnClick.Connect([this, context]()
    {
        button_play->SetActive(true);
        button_pause->SetActive(false);

        context->Resume();
    });

    button_pause->OnClick.Connect([this, context]()
    {
        button_play->SetActive(false);
        button_pause->SetActive(true);

        context->Pause();
    });

    button_hideui->OnClick.Connect([this, context]()
    {
        m_displayed = !m_displayed;
        ToggleDisplayed(m_displayed);
    });

}

void BehaviorUI::Update()
{
}

void BehaviorUI::ToggleDisplayed(bool displayed)
{
    if (displayed)
    {
        if (m_rectangleUI != nullptr) m_rectangleUI->SetToBeDisplayed(true);
        if (label_particleradius != nullptr) label_particleradius->SetToBeDisplayed(true);
        if (slider_particleradius != nullptr) slider_particleradius->SetToBeDisplayed(true);
        if (label_movementintensity != nullptr) label_movementintensity->SetToBeDisplayed(true);
        if (slider_movementintensity != nullptr) slider_movementintensity->SetToBeDisplayed(true);
        if (button_play != nullptr) button_play->SetToBeDisplayed(true);
        if (button_pause != nullptr) button_pause->SetToBeDisplayed(true);
        if (m_matrix != nullptr) m_matrix->ToggleDisplay(true);
    }
    else
    {
        if (m_rectangleUI != nullptr) m_rectangleUI->SetToBeDisplayed(false);
        if (label_particleradius != nullptr) label_particleradius->SetToBeDisplayed(false);
        if (slider_particleradius != nullptr) slider_particleradius->SetToBeDisplayed(false);
        if (label_movementintensity != nullptr) label_movementintensity->SetToBeDisplayed(false);
        if (slider_movementintensity != nullptr) slider_movementintensity->SetToBeDisplayed(false);
        if (button_play != nullptr) button_play->SetToBeDisplayed(false);
        if (button_pause != nullptr) button_pause->SetToBeDisplayed(false);
        if (m_matrix != nullptr) m_matrix->ToggleDisplay(false);
    }
}