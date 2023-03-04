#include "Button.h"


Button::Button(GraphicContext* context, const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const std::string& text)
    : GraphicObject(context, SHADER_BASIC), m_Position(position), m_Size(size), m_Color(color), m_Text(text), m_isHovered(false)
{
    // SHADER_BASIC is not the shader for it. It's just a default value.
    // TODO shader, thinking..
}

Button::~Button()
{
}

void Button::Update()
{
    // TODO
}

void Button::Draw()
{
    // TODO
}

void Button::SetPosition(const glm::vec2& position)
{
    m_Position = position;
    m_IsUpdated = false;
}

void Button::SetSize(const glm::vec2& size)
{
    m_Size = size;
    m_IsUpdated = false;
}   

void Button::SetColor(const glm::vec3& color)
{
    m_Color = color;
    m_IsUpdated = false;
}

void Button::SetText(const std::string& text)
{
    m_Text = text;
    m_IsUpdated = false;
}

void Button::SetHoveringColor(const glm::vec3& color)
{
    m_HoveringColor = color;
    m_IsUpdated = false;
}
