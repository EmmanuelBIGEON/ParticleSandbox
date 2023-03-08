#include "GraphicText.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

GraphicText::GraphicText(GraphicContext* context, const char* text, Font* font, float x, float y, float scale, glm::vec3 color) 
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_Font(font), m_x(x), m_y(y), m_scale(scale), m_Color(color), m_VAO(0), m_VBO(0)
{
    m_Shader = context->GetShader(SHADER_TEXT);
}

GraphicText::GraphicText(GraphicContext* context, const std::string& text, Font* font, float x, float y, float scale, glm::vec3 color) 
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_Font(font), m_x(x), m_y(y), m_scale(scale), m_Color(color), m_VAO(0), m_VBO(0)
{
    m_Shader = context->GetShader(SHADER_TEXT);
}

GraphicText::~GraphicText()
{
}

void GraphicText::SetText(const std::string& text)
{
    m_Text = text;
}

void GraphicText::SetFont(Font* font)
{
    m_Font = font;
}

void GraphicText::Update()
{
    // std::cout << "GraphicText::Update()" << std::endl;
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_IsUpdated = true;
}

void GraphicText::Draw()
{
    // std::cout << "GraphicText::Draw()" << std::endl;
    // https://learnopengl.com/In-Practice/Text-Rendering 
    // Code example found in learnopengl.com

    // activate corresponding render state	
    // std::cout << "Drawing with color " << m_Color.x << " " << m_Color.y << " " << m_Color.z << std::endl;
    glUniform3f(glGetUniformLocation(m_Shader->shaderID, "textColor"), m_Color.x, m_Color.y, m_Color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    float temp_x = m_x;
    float temp_y = m_y;
    // iterate through all characters
    std::string::const_iterator c;
    for (c = m_Text.begin(); c != m_Text.end(); c++) 
    {
        Character ch = m_Font->characters[*c];

        float xpos = temp_x + ch.Bearing.x * m_scale;
        float ypos = temp_y - (ch.Size.y - ch.Bearing.y) * m_scale;

        float w = ch.Size.x * m_scale;
        float h = ch.Size.y * m_scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        temp_x += (ch.Advance >> 6) * m_scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
