#include "BasicUI.h"

#include "../components/adapters/RectangleAdapter.h"
#include "../components/common/GraphicText.h"
#include "../components/ui/Button.h"
#include "../components/ui/Slider.h"

BasicUI::BasicUI(int startingX, int startingY, int width, int height)
    : m_startingX(startingX), m_startingY(startingY), m_width(width), m_height(height), m_particlesCount(0), m_fps(0)
{
}

BasicUI::~BasicUI()
{
}

void BasicUI::Init(GraphicContext* context)
{
    m_particlesCount = context->GetNbParticles_type1();
    // Create the text
    std::string text = "Particles: ";
    text += std::to_string(m_particlesCount); 

    std::string text2 = "FPS: ";
    text2 += std::to_string(m_fps);

    // -- Notice : The position calculation is horrible. --
    // Need to find a better way to calculate.
    // For now, some magic numbers are used.
    Rectangle r(Point(m_startingX, m_startingY), Point(m_startingX + m_width, m_startingY + m_height));
    m_rectangleUI = new RectangleAdapter(context, r);
    m_rectangleUI->SetColor(glm::vec3(0.2f, 0.2f, 0.2f));
    m_rectangleUI->SetOpacity(0.5f);

    float cursorY = m_startingY + m_height;
    m_particlesCountText = new GraphicText(context, text.c_str(), glm::vec2(m_startingX, cursorY), glm::vec2(m_startingX + m_width, cursorY - 25));
    cursorY -= context->font_main->GetSize();
    m_fpsText = new GraphicText(context, text2.c_str(), glm::vec2(m_startingX, cursorY), glm::vec2(m_startingX + m_width,cursorY - 25));
    cursorY -= context->font_main->GetSize()*4;
    
    label_repulsion = new GraphicText(context, "Repulsion", glm::vec2(m_startingX+ 10.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_repulsion = new Slider(context, 0.0f, 1.4f, &GraphicContext::repulsion_factor, m_startingX+ 85.0f, cursorY- 20.0f);
    cursorY -= 90.0f;
    label_attraction = new GraphicText(context, "Attraction", glm::vec2(m_startingX+ 10.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_attraction = new Slider(context, 0.0f, 1.0f, &GraphicContext::attraction_factor, m_startingX+ 85.0f, cursorY -20.0f);
    cursorY -= 90.0f;
    label_repulsiondistance = new GraphicText(context, "Repulsion Distance", glm::vec2(m_startingX+ 10.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_repulsiondistance = new Slider(context, 0.0f, 200.0f, &GraphicContext::repulsion_maximum_distance, m_startingX+ 85.0f, cursorY -40.0f);
    cursorY -= 100.0f;
    label_attractiondistance = new GraphicText(context, "Attraction Distance", glm::vec2(m_startingX+ 10.0f, cursorY+50.0f), glm::vec2(m_startingX + m_width, cursorY - 25.0f));
    slider_attractiondistance = new Slider(context, 0.0f, 700.0f, &GraphicContext::attraction_threshold_distance, m_startingX+ 85.0f, cursorY -40.0f);

    // create two buttons with play and stop icons under the basic ui.
    float cursorX = m_startingX;
    cursorY = m_startingY - 30;
    button_play = new Button(context, glm::vec2(cursorX, cursorY), glm::vec2(m_width/2, 30));
    button_play->SetPathIcon("data/img/play.png");
    button_play->SetInvertIconOnActive(true);
    button_play->SetActiveColor(glm::vec3(0.0f, 0.0f, 0.31f));

    cursorX += m_width/2;
    button_pause = new Button(context, glm::vec2(cursorX, cursorY), glm::vec2(m_width/2, 30));
    button_pause->SetPathIcon("data/img/stop.png");
    button_pause->SetInvertIconOnActive(true);
    button_pause->SetActiveColor(glm::vec3(0.0f, 0.0f, 0.31f));

    button_play->SetActive(true);

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

    std::string attraction = "Attraction: ";
    attraction += std::to_string(GraphicContext::attraction_factor);
    label_attraction->SetText(attraction.c_str());

    std::string repulsiondistance = "Repulsion Distance: ";
    repulsiondistance += std::to_string(GraphicContext::repulsion_maximum_distance);
    label_repulsiondistance->SetText(repulsiondistance.c_str());

    std::string attractiondistance = "Attraction Distance: ";
    attractiondistance += std::to_string(GraphicContext::attraction_threshold_distance);
    label_attractiondistance->SetText(attractiondistance.c_str());


    m_particlesCountText->SetText(text.c_str());
    m_fpsText->SetText(text2.c_str());

}
