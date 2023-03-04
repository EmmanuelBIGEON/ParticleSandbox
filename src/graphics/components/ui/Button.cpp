#include "Button.h"

#include <glad/glad.h>

Button::Button(GraphicContext* context, const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const std::string& text)
    : GraphicObject(context, SHADER_BASIC), m_Pos(position), m_Size(size), m_Color(color), m_Text(text), m_isHovered(false),
    m_VAO(0), m_VBO(0), m_EBO(0), m_TextObject(nullptr)
{
    // SHADER_BASIC is not the shader for it. It's just a default value.
    // TODO shader, thinking..
    m_Shader = m_Context->GetShader(SHADER_BASIC);
}

Button::~Button()
{
}

void Button::Update()
{
    if(m_VAO == 0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
    }
    
    // Default form button is a rectangle
    // x                                y
    m_vertices[0] = m_Pos.x;            m_vertices[1] = m_Pos.y; 
    m_vertices[2] = m_Pos.x + m_Size.x; m_vertices[3] = m_Pos.y;
    m_vertices[4] = m_Pos.x + m_Size.x; m_vertices[5] = m_Pos.y + m_Size.y;
    m_vertices[6] = m_Pos.x;            m_vertices[7] = m_Pos.y + m_Size.y;

    m_indices[0] = 0; m_indices[1] = 1; m_indices[2] = 2;
    m_indices[3] = 0; m_indices[4] = 2; m_indices[5] = 3;

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // m_TextObject later..


}

void Button::Draw()
{
    glBindVertexArray(m_VAO);
    if(m_isHovered)
        m_Shader->SetVec3("buttonColor", m_HoveringColor);
    else
        m_Shader->SetVec3("buttonColor", m_Color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
}

void Button::SetPosition(const glm::vec2& position)
{
    m_Pos = position;
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
