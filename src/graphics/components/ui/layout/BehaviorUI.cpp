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
    m_particlesCountText(nullptr), m_fpsText(nullptr), m_fps(0), m_particlesCount(0)
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

    cursorY -= 40.0f;
    
    button_play = new Button(context, glm::vec2(30.0f, cursorY), glm::vec2(m_width/2, 30));
    button_play->SetPathIcon("data/img/play.png");
    button_play->SetInvertIconOnActive(true);
    button_play->SetActiveColor(glm::vec3(0.0f, 0.0f, 0.31f));

    button_pause = new Button(context, glm::vec2(30.0f + m_width/2, cursorY), glm::vec2(m_width/2, 30));
    button_pause->SetPathIcon("data/img/stop.png");
    button_pause->SetInvertIconOnActive(true);
    button_pause->SetActiveColor(glm::vec3(0.0f, 0.0f, 0.31f));
    button_play->SetActive(true);

    cursorY -= 10.0f;

    std::string text = "Particles: " + std::to_string(0); 
    std::string text2 = "FPS: " + std::to_string(0);
    m_particlesCountText = new GraphicText(context, text.c_str(), glm::vec2(30.0f, cursorY), glm::vec2(30.0f + m_width, cursorY - 25));
    m_particlesCountText->SetAlignment(TextAlign::ALIGN_LEFT);
    cursorY -= context->font_main->GetSize();
    m_fpsText = new GraphicText(context, text2.c_str(), glm::vec2(30.0f, cursorY), glm::vec2(30.0f + m_width,cursorY - 25));
    m_fpsText->SetAlignment(TextAlign::ALIGN_LEFT);
    cursorY -= 160.0f;
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
    
    // Connect the watcher to the context
    m_particleAddedSlot = context->OnParticlesAdded.Connect([this](int nb)
    {
        m_particlesCount += nb;
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

}

void BehaviorUI::Update()
{
    m_fps = Application::fps;

    std::string text = "Particles: ";
    text += std::to_string(m_particlesCount);

    std::string text2 = "FPS: ";
    text2 += std::to_string(m_fps);

    m_particlesCountText->SetText(text.c_str());
    m_fpsText->SetText(text2.c_str());
}
