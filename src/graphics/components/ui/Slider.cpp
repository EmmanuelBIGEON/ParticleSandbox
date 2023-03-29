#include "Slider.h"

#include <iostream>
#include <glad/glad.h>

#define REFVALUE *m_referencedValue

float Slider::height = 5.0f;
float Slider::width = 100.0f;

Slider::Slider(GraphicContext* context, float min, float max, float* value, float x, float y)
    : GraphicObject(context,SHADER_UI), m_Min(min), m_Max(max), m_referencedValue(value), m_shiftX(x), m_shiftY(y),
     m_EBO(0), m_VAO(0), m_VBO(0), isHovered(false), isPressed(false),
     m_shiftCursorX(0.0f), m_shiftCursorY(0.0f), m_VAO_cursor(0), m_VBO_cursor(0), m_EBO_cursor(0)
{
    m_Shader = context->GetShader(AvailableShader::SHADER_UI);
    m_Context->OnMouseMoved.Connect([this](float x, float y) { OnMouseMoved(x, y); });
    m_Context->OnMousePressed.Connect([this](float x, float y) { OnMousePressed(x, y); });
    m_Context->OnMouseReleased.Connect([this](float x, float y) { OnMouseReleased(x, y); });
}

Slider::~Slider()
{
}

void Slider::Update()
{
    if(m_IsUpdated) return;

    // draw a rectangle that represents the slider

    // Give (x,y) coordinates to the vertices 
    m_vertices[0] = (-width / 2.0f); // Corner
    m_vertices[1] = (-height / 2.0f); // bottom left
    m_vertices[2] = (-width / 2.0f); // Corner
    m_vertices[3] = (height / 2.0f); // top left
    m_vertices[4] = (width / 2.0f); // Corner
    m_vertices[5] = (height / 2.0f); // top right
    m_vertices[6] = (width / 2.0f); // Corner
    m_vertices[7] = (-height / 2.0f); // bottom right
    // Assign elements to the vertices
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // Create the cursor vertices
    float cursorWidth = 14.0f;
    float cursorHeight = 20.0f;
    m_shiftCursorX = (m_shiftX- width/2.0f) + (REFVALUE - m_Min) / (m_Max - m_Min) * width;
    m_shiftCursorY = m_shiftY;
    m_cursorVertices[0] = (-cursorWidth / 2.0f); // Corner
    m_cursorVertices[1] = (-cursorHeight / 2.0f); // bottom left
    m_cursorVertices[2] = (-cursorWidth / 2.0f); // Corner
    m_cursorVertices[3] = (cursorHeight / 2.0f); // top left
    m_cursorVertices[4] = (cursorWidth / 2.0f); // Corner
    m_cursorVertices[5] = (cursorHeight / 2.0f); // top right
    m_cursorVertices[6] = (cursorWidth / 2.0f); // Corner
    m_cursorVertices[7] = (-cursorHeight / 2.0f); // bottom right

    float padding = 2.0f;
    m_boundingBoxCursor[0] = m_shiftCursorX - cursorWidth / 2.0f - padding;
    m_boundingBoxCursor[1] = m_shiftCursorY - cursorHeight / 2.0f - padding;
    m_boundingBoxCursor[2] = m_shiftCursorX + cursorWidth / 2.0f + padding;
    m_boundingBoxCursor[3] = m_shiftCursorY + cursorHeight / 2.0f + padding;


    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // --- Prepare the slider
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), m_vertices, GL_STATIC_DRAW);
    // x and y coordinates
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // --- Prepare the cursor 
    glGenVertexArrays(1, &m_VAO_cursor);
    glGenBuffers(1, &m_VBO_cursor);
    glGenBuffers(1, &m_EBO_cursor);
    glBindVertexArray(m_VAO_cursor);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_cursor);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), m_cursorVertices, GL_STATIC_DRAW);
    // x and y coordinates
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_cursor);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    m_IsUpdated = true;
}

void Slider::Draw()
{
    if(!m_IsUpdated) return;

    // draw the slider
    m_Shader->Use();
    m_Shader->SetVec3("elementColor", glm::vec3(0.74f, 0.74f, 0.74f));
    m_Shader->SetVec2("shiftPos", glm::vec2(m_shiftX, m_shiftY));
    m_Shader->SetFloat("shiftZ", 1.0f); 
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // draw the cursor
    if(!isHovered)   m_Shader->SetVec3("elementColor", glm::vec3(0.77f, 0.77f, 0.77f));
    else            m_Shader->SetVec3("elementColor", glm::vec3(0.92f, 0.92f, 0.92f));

    m_Shader->SetVec2("shiftPos", glm::vec2(m_shiftCursorX, m_shiftCursorY));
    glBindVertexArray(m_VAO_cursor);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


void Slider::OnMouseMoved(float x, float y)
{
    if(!m_IsUpdated) return;

    if(isPressed)
    {
        m_shiftCursorX = x;
        float temp = (m_shiftCursorX - m_shiftX + width/2.0f) * (m_Max - m_Min) / width;
        if (temp < m_Min) temp = m_Min;
        if (temp > m_Max) temp = m_Max;
        REFVALUE = temp;
        isHovered = true; 
        m_IsUpdated = false;

    }else 
    {
        if (x > m_boundingBoxCursor[0] && x < m_boundingBoxCursor[2] &&
            y > m_boundingBoxCursor[1] && y < m_boundingBoxCursor[3])
        {
            isHovered = true;
        }
        else isHovered = false;
    }

}

void Slider::OnMousePressed(float x, float y)
{
    if(isHovered)
        isPressed = true;
}

void Slider::OnMouseReleased(float x, float y)
{
    isPressed = false;
}