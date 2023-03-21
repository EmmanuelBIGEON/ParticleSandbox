#include "GraphicText.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>


// Defining the minimal padding of the box containing the text.
#define FIXED_PADDING 5

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

GraphicText::GraphicText(GraphicContext* context, const char* text, Font* font, float xstart, float ystart, float xend, float yend, glm::vec3 color)
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_Font(font), m_xstart(xstart), m_ystart(ystart), m_xend(xend), m_yend(yend), m_scale(1.0f), m_Color(color), m_VAO(0), m_VBO(0)
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

    // // Calculate size with xstart, ystart, xend, yend
    // // It needs to stay in the box. Scale if needed.
    // // For starter, TextAlign is always center
    // int text_align = TextAlign::ALIGN_CENTER;

    // // Calculate size of all characters
    // // to determine if text is too large and to align it accordingly
    // float textWidth = 0.0f;
    // std::string::const_iterator c;
    // for (c = m_Text.begin(); c != m_Text.end(); c++) 
    // {
    //     Character ch = m_Font->characters[*c];
    //     textWidth += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
    // }


    // // check if text too large
    // // If text overflows box, if height is large enough, create the text on multiple lines
    // if (textWidth > (m_xend - m_xstart))
    // {

    //     // Get the font size (in pixels)
    //     int fontSize = m_Font->GetSize();

    //     // Calculate the number of lines needed
    //     int nbLines = ceil(textWidth / (m_xend - m_xstart));

    //     // Calculate lines available in box height  
    //     int availableLines = (m_yend - m_ystart - FIXED_PADDING * 2) / fontSize;
    //     if(availableLines <= 0)
    //     {
    //         std::cout << "Box of text too small unable to display anything (text value: " << m_Text << ")" << std::endl;
    //         m_IsUpdated = true;
    //         return;
    //     }
        
    //     // The maximum difference between available lines and needed lines is 3
    //     // Exceeded this limit, the text won't be displayed because it will be too small.
    //     // Otherwise, the text will be scaled down to fit the box if it is between 1 and 3 lines exceeding the limit.
    //     if (nbLines - availableLines > 3) // Too much lines, text won't be displayed
    //     {
    //         std::cout << "Text too large, not displayed (text value: " << m_Text << ")" << std::endl;
    //         m_IsUpdated = true;
    //         return;
    //     }
    //     else if (nbLines - availableLines > 0) // Text will be scaled down to fit the available space
    //     {
    //         // Calculate the new scale using the available space in all lines
    //         int maxAvailableSpace = nbLines * (m_xend - m_xstart - FIXED_PADDING * 2);
    //         m_scale = (float)maxAvailableSpace / textWidth;
    //     }else
    //     {
    //         m_scale = 1.0f;
    //     }

    //     // Create the vertices for each line
    //     // Considering text-align and the scale previously calculated
    //     // TODO...

    // }

    

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
