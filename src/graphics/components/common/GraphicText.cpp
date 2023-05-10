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

GraphicText::GraphicText(GraphicContext* context, const char* text, glm::vec2 topLeft, glm::vec2 bottomRight)
    : GraphicObject(context,SHADER_TEXT), m_Text(text), m_scale(1.0f), m_VAO(0), m_VBO(0), m_vertices(nullptr), m_nbCharacters(0), m_Alignment(TextAlign::ALIGN_CENTER) 
{
    m_Font = context->font_main;
    m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
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
    m_IsUpdated = false;
}

void GraphicText::SetFont(Font* font)
{
    m_Font = font;
    m_IsUpdated = false;
}

void GraphicText::Update()
{
    if(m_Text.empty())
    {
        m_IsUpdated = true;
        return;
    }
    m_lines.clear();

    // Calculate size with xstart, ystart, xend, yend
    // It needs to stay in the box. Scale if needed.
    // For starter, TextAlign is always center
    TextAlign text_align = m_Alignment;

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

    int lineWidth = m_xend - m_xstart - FIXED_PADDING * 2;
    int boxHeight = m_ystart - m_yend - FIXED_PADDING * 2;
    m_scale = 1.0f; // default value. 
    // check if text too large
    if (textWidth > (m_xend - m_xstart))
    {

        // Calculate the number of lines needed (textWidth / available space on x axis for one line)
        int nbLinesNeeded = ceil(textWidth / lineWidth);

        // Calculate lines available in box height  
        int availableLines =  boxHeight / textHeight;

        // Textheight is too high for the box
        if(availableLines < 0 || nbLinesNeeded - availableLines > 0) // Text will be scaled down
        {
            // find a height that gives enough lines for the text to be displayed
            // reduce line height until it fits
            float originalTextHeight = textHeight;
            float tempTextHeight = textHeight;
            while (nbLinesNeeded - availableLines > 0)
            {
                tempTextHeight -= 1.0f;
                m_scale = tempTextHeight / originalTextHeight;

                // recalculate needed lines
                nbLinesNeeded = ceil((textWidth*m_scale*1.2f) / (lineWidth)); // Apply a little padding (1.2f) to reduce number of mistakes
                availableLines =  boxHeight / tempTextHeight;
            }
        }

        // -- Split the text into words separated by spaces --
        int font_spacesize = m_Font->characters[' '].Advance >> 6;
        std::vector<std::string> words;
        std::string word;
        std::istringstream iss(m_Text);
        // operator >> is used to read a word from the stream
        while (iss >> word) words.push_back(word);
        // ---------------------------------------------------

        // Create the lines
        std::string line;
        float currentLineWidth = 0.0f;
        for (int i = 0; i < words.size(); i++)
        {
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

    m_vertices = new float[6 * 4 * m_nbCharacters];
    int index = 0; // current index in vertices array
    int index2 = 0;
    for (int i = 0; i < m_linesToDraw; i++)
    {
        float currentLineWidth = 0.0f;
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
            currentLineWidth += (ch.Advance >> 6) * m_scale;

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

        // apply alignment
        float offset = (lineWidth - currentLineWidth);

        if(text_align == ALIGN_CENTER && offset>0.0f)
        {
            offset /= 2.0f;

            while(index2 < index)
            {
                m_vertices[index2] += offset;
                index2 += 4;
            }
        }
        else if(text_align == ALIGN_RIGHT && offset>0.0f)
        {
            while(index2 < index)
            {
                m_vertices[index2] += offset;
                index2 += 4;
            }
        }
    }

    // calculate remaining height
    m_remainingHeight = boxHeight - (m_textScaledHeight * m_linesToDraw) - (FIXED_PADDING * 2);
    float offset = m_remainingHeight / 2.0f;
    // auto apply vertical alignment
    if(m_remainingHeight > 0)
    {
        int index =0;
        for (int i = 0; i < m_linesToDraw; i++)
        {
            std::string line = m_lines[i];
            for (std::string::const_iterator c = line.begin(); c != line.end(); c++) 
            {
                m_vertices[(index * 6 * 4) + 1] -= offset;
                m_vertices[(index * 6 * 4) + 5] -= offset;
                m_vertices[(index * 6 * 4) + 9] -= offset;
                m_vertices[(index * 6 * 4) + 13] -= offset;
                m_vertices[(index * 6 * 4) + 17] -= offset;
                m_vertices[(index * 6 * 4) + 21] -= offset;
                index++;
            }
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
    if(m_Text.empty())
        return;
        
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
