#include "BasicUI.h"

BasicUI::BasicUI(int startingX, int startingY, int width, int height)
    : m_startingX(startingX), m_startingY(startingY), m_width(width), m_height(height), m_particlesCount(0), m_fps(0)
{
}

BasicUI::~BasicUI()
{
}

void BasicUI::Init(GraphicContext* context)
{
    m_particlesCount = context->GetNbParticles();
    // Create the text
    std::string text = "Particles: ";
    text += std::to_string(m_particlesCount); 

    std::string text2 = "FPS: ";
    text2 += std::to_string(m_fps);

    m_particlesCountText = new GraphicText(context, text.c_str(), glm::vec2(m_startingX, m_startingY + m_height), glm::vec2(m_startingX + m_width, m_startingY));
    m_fpsText = new GraphicText(context, text2.c_str(), glm::vec2(m_startingX, m_startingY + m_height - 25), glm::vec2(m_startingX + m_width, m_startingY - 25));

    // Connect the watcher to the context
    m_particleAddedSlot = context->OnParticlesAdded.Connect([this](int nb)
    {
        m_particlesCount += nb;
    });
}

void BasicUI::Update()
{
    m_fps = Application::fps;

    std::string text = "Particles: ";
    text += std::to_string(m_particlesCount);

    std::string text2 = "FPS: ";
    text2 += std::to_string(m_fps);

    m_particlesCountText->SetText(text.c_str());
    m_fpsText->SetText(text2.c_str());

}
