#include "BasicUI.h"

#include "../../adapters/RectangleAdapter.h"
#include "../../common/GraphicText.h"
#include "../Button.h"
#include "../Slider.h"
#include "../Checkbox.h"

BasicUI::BasicUI(int startingX, int startingY, int width, int height)
    : m_startingX(startingX), m_startingY(startingY), m_width(width), m_height(height), m_particlesCount(0), m_fps(0)
{
}

BasicUI::~BasicUI()
{
}

void BasicUI::Init(GraphicContext* context)
{

    Rectangle r(Point(m_startingX, m_startingY), Point(m_startingX + m_width, m_startingY + m_height));
    m_rectangleUI = new RectangleAdapter(context, r);
    m_rectangleUI->SetColor(glm::vec3(0.2f, 0.2f, 0.2f));
    m_rectangleUI->SetOpacity(0.5f);

    m_particlesCount =0;
    std::string text = "Particles: ";
    text += std::to_string(m_particlesCount); 
    std::string text2 = "FPS: ";
    text2 += std::to_string(m_fps);
    float cursorY = m_startingY + m_height;
    m_particlesCountText = new GraphicText(context, text.c_str(), glm::vec2(m_startingX, cursorY), glm::vec2(m_startingX + m_width, cursorY - 25));
    cursorY -= context->font_main->GetSize();
    m_fpsText = new GraphicText(context, text2.c_str(), glm::vec2(m_startingX, cursorY), glm::vec2(m_startingX + m_width,cursorY - 25));
    cursorY -= context->font_main->GetSize()*3.0;
    
    label_repulsion = new GraphicText(context, "Repulsion", glm::vec2(m_startingX+ 8.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_repulsion = new Slider(context, 0.0f, 1.4f, &GraphicContext::repulsion_factor, m_startingX+ 85.0f, cursorY- 20.0f);
    cursorY -= 75.0f;
    label_force = new GraphicText(context, "force", glm::vec2(m_startingX+ 8.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_force = new Slider(context, 0.0f, 0.7f, &GraphicContext::force_factor, m_startingX+ 85.0f, cursorY -20.0f);
    cursorY -= 80.0f;
    label_repulsiondistance = new GraphicText(context, "Repulsion Distance", glm::vec2(m_startingX+ 8.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_repulsiondistance = new Slider(context, 0.0f, 200.0f, &GraphicContext::repulsion_maximum_distance, m_startingX+ 85.0f, cursorY -40.0f);
    cursorY -= 100.0f;
    label_forcedistance = new GraphicText(context, "force Distance", glm::vec2(m_startingX+ 8.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_forcedistance = new Slider(context, 0.0f, 700.0f, &GraphicContext::force_threshold_distance, m_startingX+ 85.0f, cursorY -40.0f);
    cursorY -= 90.0f;
    label_particleradius = new GraphicText(context, "Particle Radius", glm::vec2(m_startingX+ 8.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_particleradius = new Slider(context, 0.0f, 10.0f, &particleRadius, m_startingX+ 85.0f, cursorY -10.0f);
    cursorY -= 65.0f;
    label_movementintensity = new GraphicText(context, "Movement intensity", glm::vec2(m_startingX+ 8.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_movementintensity = new Slider(context, 0.0f, 0.25f, &GraphicContext::movement_intensity, m_startingX+ 85.0f, cursorY -20.0f);
    cursorY -= 75.0f;
    label_useVelocity = new GraphicText(context, "Use Velocity:", glm::vec2(m_startingX+ 6.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 35.0f));
    checkbox_velocityActivation = new Checkbox(context, &GraphicContext::useVelocity,glm::vec2(m_startingX + m_width - 25.0f, cursorY));
    cursorY -= 100.0f;
    
    // create two buttons with play and stop icons under the basic ui.
    float cursorX = m_startingX;
    cursorY = m_startingY - 30;
    button_play = new Button(context, glm::vec2(cursorX, cursorY), glm::vec2(m_width/2, 30));
    button_play->SetPathIcon("data/img/play.png");
    button_play->SetInvertIconOnActive(true);
    button_play->SetActiveColor(glm::vec3(0.0f, 0.2f, 0.71f));

    cursorX += m_width/2;
    button_pause = new Button(context, glm::vec2(cursorX, cursorY), glm::vec2(m_width/2, 30));
    button_pause->SetPathIcon("data/img/stop.png");
    button_pause->SetInvertIconOnActive(true);
    button_pause->SetActiveColor(glm::vec3(0.0f, 0.2f, 0.71f));
    button_play->SetActive(true);

    // update particle size
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

void BasicUI::Update()
{
    m_fps = Application::fps;

    std::string text = "Particles: ";
    text += std::to_string(m_particlesCount);

    std::string text2 = "FPS: ";
    text2 += std::to_string(m_fps);

    std::string repulsion = "Repulsion: ";
    repulsion += std::to_string(GraphicContext::repulsion_factor);
    label_repulsion->SetText(repulsion.c_str());

    std::string force = "force: ";
    force += std::to_string(GraphicContext::force_factor);
    label_force->SetText(force.c_str());

    std::string repulsiondistance = "Repulsion Distance: ";
    repulsiondistance += std::to_string(GraphicContext::repulsion_maximum_distance);
    label_repulsiondistance->SetText(repulsiondistance.c_str());

    std::string forcedistance = "force Distance: ";
    forcedistance += std::to_string(GraphicContext::force_threshold_distance);
    label_forcedistance->SetText(forcedistance.c_str());


    m_particlesCountText->SetText(text.c_str());
    m_fpsText->SetText(text2.c_str());

}
