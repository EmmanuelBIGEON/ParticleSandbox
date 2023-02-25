#pragma once

#include "BezierCurve.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/ext.hpp>

#include <iostream>

BezierCurve::BezierCurve(GraphicContext* context, const Line& line) : GraphicObject(context, SHADER_LINE), m_Line(line), m_Color(1.0f, 0.0f, 0.0f), m_VAO(0), m_VBO(0), m_vertices(nullptr)
{
    m_Shader = context->GetShader(SHADER_LINE);
}

BezierCurve::~BezierCurve()
{
    // delete[] m_vertices;
}

void BezierCurve::SetColor(const glm::vec3& color)
{
    m_Color = color;
    m_IsUpdated=false;
}

void BezierCurve::SetLine(const Line& line)
{
    m_Line = line;
    m_IsUpdated=false;
}


void BezierCurve::Update()
{
    if(m_vertices)
    {
        delete[] m_vertices;
    }

    glm::vec3 P0(m_Line.start.x, m_Line.start.y, 0.0f);
    glm::vec3 P1(m_ControlPoint1.x, m_ControlPoint1.y, 0.0f);
    glm::vec3 P2(m_ControlPoint2.x, m_ControlPoint2.y, 0.0f);
    glm::vec3 P3(m_Line.end.x, m_Line.end.y, 0.0f);

    std::cout << "Drawing Bezier curve from " << P0.x << ", " << P0.y << " to " << P3.x << ", " << P3.y << " with control points " << P1.x << ", " << P1.y << " and " << P2.x << ", " << P2.y << std::endl;

    // Calculate the intermediate points
    // Change NUM_POINTS to change the quality of the curve
    const int NUM_POINTS = 100;

    m_vertices = new float[NUM_POINTS * 2];

    for(int i = 0; i < NUM_POINTS; i++) 
    {
        float t = static_cast<float>(i) / (NUM_POINTS - 1);
        glm::vec3 position = (1 - t) * (1 - t) * (1 - t) * P0 +
                             3 * (1 - t) * (1 - t) * t * P1 +
                             3 * (1 - t) * t * t * P2 +
                             t * t * t * P3;
                             
        m_vertices[i * 2] = position.x;
        m_vertices[i * 2 + 1] = position.y;

    }


    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUM_POINTS * 2, m_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glEnableVertexAttribArray(0);

    m_IsUpdated = true;

}

void BezierCurve::Draw()
{
    m_Shader->SetVec3("lineColor", m_Color);
    glBindVertexArray(m_VAO);
    // glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_LINE_STRIP, 0, 100);
    glBindVertexArray(0);
}
