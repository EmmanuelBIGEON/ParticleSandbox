#include "GraphicText.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

#include "../adapters/RectangleAdapter.h"


// Defining the minimal padding of the box containing the text.
#define FIXED_PADDING 5

GraphicText::GraphicText(GraphicContext* context, const char* text, Font* font, float x, float y, float scale, glm::vec3 color) 
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_Font(font), m_x(x), m_y(y), m_scale(scale), m_Color(color), m_VAO(0), 
    m_VBO(0), m_vertices(nullptr), m_nbCharacters(0)
{
    m_Shader = context->GetShader(SHADER_TEXT);
}

GraphicText::GraphicText(GraphicContext* context, const std::string& text, Font* font, float x, float y, float scale, glm::vec3 color) 
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_Font(font), m_x(x), m_y(y), m_scale(scale), m_Color(color), m_VAO(0), 
    m_VBO(0), m_vertices(nullptr), m_nbCharacters(0)
{
    m_Shader = context->GetShader(SHADER_TEXT);
}

GraphicText::GraphicText(GraphicContext* context, const char* text, Font* font, float xstart, float ystart, float xend, float yend, glm::vec3 color)
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_Font(font), m_xstart(xstart), m_ystart(ystart), m_xend(xend), 
    m_yend(yend), m_scale(1.0f), m_Color(color), m_VAO(0), m_VBO(0), m_vertices(nullptr), m_nbCharacters(0)
{
    m_Shader = context->GetShader(SHADER_TEXT);
}

GraphicText::GraphicText(GraphicContext* context, const char* text, glm::vec2 topLeft, glm::vec2 bottomRight)
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_scale(1.0f), m_VAO(0), m_VBO(0), m_vertices(nullptr), m_nbCharacters(0)
{
    m_Font = context->font_main;
    m_Color = glm::vec3(0.0f, 1.0f, 0.0f);
    m_xstart = topLeft.x;
    m_ystart = topLeft.y;
    m_xend = bottomRight.x;
    m_yend = bottomRight.y;
    m_linesToDraw = 0;
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
    // Calculate size with xstart, ystart, xend, yend
    // It needs to stay in the box. Scale if needed.
    // For starter, TextAlign is always center
    int text_align = TextAlign::ALIGN_CENTER;

    // Calculate size of all characters
    // to determine if text is too large and to align it accordingly
    float textWidth = 0.0f;
    float textHeight = m_Font->GetSize();
    m_nbCharacters = 0;
    std::string::const_iterator c;
    for (c = m_Text.begin(); c != m_Text.end(); c++) 
    {
        Character ch = m_Font->characters[*c];
        textWidth += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        m_nbCharacters++;
    }
    std::cout << "Text width of " << m_Text << " is " << textWidth << std::endl;

    // check if text too large
    // If text overflows box, if height is large enough, create the text on multiple lines
    if (textWidth > (m_xend - m_xstart))
    {
        std::cout << "Text too large, will be scaled down (text value: " << m_Text << ")" << std::endl;

        // Calculate the number of lines needed
        int nbLinesNeeded = ceil(textWidth / (m_xend - m_xstart));
        std::cout << "Text need " << nbLinesNeeded << " lines" << std::endl;

        // Calculate lines available in box height  
        int lineWidth = m_xend - m_xstart - FIXED_PADDING * 2;
        int availableLines = (m_ystart - m_yend - FIXED_PADDING * 2) / textHeight;
        std::cout << "Available lines: " << availableLines << std::endl;

        m_scale = 1.0f;
        if(availableLines < 0)
        {
            // Calculate minimum scale to fit the box
            float availableWidth = lineWidth * nbLinesNeeded;
            m_scale = availableWidth / textWidth;

        }else if (nbLinesNeeded - availableLines > 0) // Text will be scaled down
        {
            // Calculate minimum scale to fit the box
            float availableWidth = lineWidth * availableLines;
            m_scale = availableWidth / textWidth;
        }
        
        

        m_lines.clear();
        // Split the text into words separated by spaces
        int font_spacesize = m_Font->characters[' '].Advance >> 6;
        std::vector<std::string> words;
        std::string word;
        std::istringstream iss(m_Text);
        // operator >> is used to read a word from the stream
        while (iss >> word) words.push_back(word);

        // Create the lines
        std::string line;
        float currentLineWidth = 0.0f;
        for (int i = 0; i < words.size(); i++)
        {
            std::cout << "Word " << i << ": " << words[i] << std::endl;
            float wordWidth = 0.0f;
            std::string::const_iterator cword;
            for (cword = words[i].begin(); cword != words[i].end(); cword++) 
            {
                Character ch = m_Font->characters[*cword];
                wordWidth += (ch.Advance >> 6) * m_scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            }

            // If the line is empty, add the word
            if (line.empty()) { line = words[i]; currentLineWidth = wordWidth; continue; }

            // If the line is not empty, check if adding the word will make the line too long
            // If it is, add the line to the vector and start a new line with the word
            // Otherwise, add the word to the line
            if ((currentLineWidth + font_spacesize + wordWidth) > lineWidth)
            {
                m_lines.push_back(line);
                line = words[i];
                currentLineWidth = wordWidth;
            }
            else
            {
                line += " " + words[i];
                currentLineWidth += font_spacesize + wordWidth;
            }
        }
        // Add the last line
        if(!line.empty()) m_lines.push_back(line);

        for (int i = 0; i < m_lines.size(); i++) std::cout << "Line " << i << ": " << m_lines[i] << std::endl;

        m_linesToDraw = m_lines.size();
        m_textScaledHeight = textHeight * m_scale;
    }else 
    {
        // text ok on one line
        m_linesToDraw = 1;
        m_lines.clear();
        m_lines.push_back(m_Text);
        m_scale = 1.0f;
        m_textScaledHeight = textHeight;

    }
    
    // prepares vertices
    if(m_vertices)
    {
        delete[] m_vertices;
        m_vertices = nullptr;
    }

    std::cout << "Number of characters: " << m_nbCharacters << std::endl;
    m_vertices = new float[6 * 4 * m_nbCharacters];
    int index = 0; // current index in vertices array
    for (int i = 0; i < m_linesToDraw; i++)
    {
        std::string line = m_lines[i];
        float x = m_xstart + FIXED_PADDING;
        float y = m_ystart - FIXED_PADDING - (m_textScaledHeight * (i+1));
        for (std::string::const_iterator c = line.begin(); c != line.end(); c++) 
        {
            Character ch = m_Font->characters[*c];
            float xpos = x + ch.Bearing.x * m_scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * m_scale;
            float w = ch.Size.x * m_scale;
            float h = ch.Size.y * m_scale;

            // fill m_vertices
            m_vertices[index++] = xpos;     m_vertices[index++] = ypos + h;
            m_vertices[index++] = 0.0f;     m_vertices[index++] = 0.0f;

            m_vertices[index++] = xpos;     m_vertices[index++] = ypos;
            m_vertices[index++] = 0.0f;     m_vertices[index++] = 1.0f;

            m_vertices[index++] = xpos + w; m_vertices[index++] = ypos;
            m_vertices[index++] = 1.0f;     m_vertices[index++] = 1.0f;

            m_vertices[index++] = xpos;     m_vertices[index++] = ypos + h;
            m_vertices[index++] = 0.0f;     m_vertices[index++] = 0.0f;

            m_vertices[index++] = xpos + w; m_vertices[index++] = ypos;
            m_vertices[index++] = 1.0f;     m_vertices[index++] = 1.0f;

            m_vertices[index++] = xpos + w; m_vertices[index++] = ypos + h;
            m_vertices[index++] = 1.0f;     m_vertices[index++] = 0.0f;
            
            // advance
            x += (ch.Advance >> 6) * m_scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    


    m_IsUpdated = true;
}

void GraphicText::Draw()
{
    glUniform3f(glGetUniformLocation(m_Shader->shaderID, "textColor"), m_Color.x, m_Color.y, m_Color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    int index = 0;
    for(int i = 0; i < m_linesToDraw; i++)
    {
        const std::string& line = m_lines[i];
        for (std::string::const_iterator c = line.begin(); c != line.end(); c++) 
        {
            const Character& ch = m_Font->characters[*c];
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 4, &m_vertices[index * 6 * 4]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            index += 1;
        }
    }
}
