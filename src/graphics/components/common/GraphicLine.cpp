#include "GraphicLine.h"

#include <glad/glad.h>

#include <iostream>

GraphicLine::GraphicLine(GraphicContext* context, const Line& line, const glm::vec3& color)
    : GraphicObject(context, AvailableShader::SHADER_LINE), m_Line(line), m_Color(color), m_VAO(0), m_VBO(0)
{
    m_rotationMatrixInversed = glm::mat2(0.0f);
    m_Shader = context->GetShader(AvailableShader::SHADER_LINE);
}

GraphicLine::~GraphicLine()
{
}

void GraphicLine::Update()
{
    m_vertices[0] = m_Line.start.x;
    m_vertices[1] = m_Line.start.y;
    m_vertices[2] = m_Line.end.x;
    m_vertices[3] = m_Line.end.y;


    std::cout << "Drawing a line from " << m_Line.start.x << ", " << m_Line.start.y << " to " << m_Line.end.x << ", " << m_Line.end.y << std::endl;

    // VAO  
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    m_IsUpdated = true;
}

void GraphicLine::Draw()
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
