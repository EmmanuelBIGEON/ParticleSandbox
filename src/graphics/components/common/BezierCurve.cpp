#pragma once

#include "BezierCurve.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/ext.hpp>

#include <iostream>

BezierCurve::BezierCurve(GraphicContext* context, const Line& line) : GraphicObject(context, SHADER_LINE), m_Line(line), m_vertices(nullptr), m_Color(1.0f, 0.0f, 0.0f)
{
    m_Shader = context->GetShader(SHADER_LINE);
}

BezierCurve::~BezierCurve()
{
    delete[] m_vertices;
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
    
    // Rotate the control points to be able tu use the bezier formula
    // Get the vector between the start and end point to calculate the angle
    glm::vec3 v = P3 - P0;
    // Get the angle between the vector and the x-axis
    // float angle = atan2(v.y, v.x);
    float angle = atan2(P0.y, P0.x);
    angle = -angle;
    std::cout <<"angle: " << angle << std::endl;
    // convert to degree
    // angle = glm::degrees(angle);
    // angle = -angle;
    std::cout <<"angle (degre): " << angle << std::endl;
    // Rotate the points to be able to use the bezier formula
    // https://fr.wikipedia.org/wiki/Changement_de_base_(alg%C3%A8bre_lin%C3%A9aire)
    glm::mat3 rotationMatrix = glm::mat3(
        {cos(angle), -sin(angle), 0},
        {sin(angle), cos(angle), 0},
        {0, 0, 1});
    // glm::mat3 rotationMatrix = glm::mat3(cos(0), -sin(0), 0, sin(0), cos(0), 0, 0, 0, 1);
    // glm::mat2 rotationMatrix = glm::mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    // rotate from the origin
    float test = P0.x * cos(angle) - P0.y * (sin(angle));
    float test2 = P0.x * sin(angle) + P0.y * cos(angle);
    // tranform vec3 into column vector

    // rotate from the origin
    std::cout << "new P0: " << P0.x << ", " << P0.y << std::endl;
    std::cout << " test = " << test << std::endl;
    std::cout << " test2 = " << test2 << std::endl;
    std::cout << "cos : " << cos(angle) << std::endl;
    std::cout << "sin : " << sin(angle) << std::endl;
    P0 = P0 * rotationMatrix;
    P1 = P1 *rotationMatrix;

    std::cout << "new P1: " << P1.x << ", " << P1.y << std::endl;
    P2 = P2 * rotationMatrix;
    std::cout << "new P2: " << P2.x << ", " << P2.y << std::endl;
    P3 = P3 * rotationMatrix;
    std::cout << "new P3: " << P3.x << ", " << P3.y << std::endl;
    
    // Register the reversed rotation matrix to rotate the points back in the vertex shader
    m_rotationMatrixInversed = glm::inverse(rotationMatrix);

    // test if reverse is okay with P0
    test = P0.x * cos(-angle) - P0.y * (sin(-angle));
    test2 = P0.x * sin(-angle) + P0.y * cos(-angle);
    P0 = P0 * m_rotationMatrixInversed;

    std::cout << " test = " << test << std::endl;
    std::cout << " test2 = " << test2 << std::endl;
    std::cout << "new P0: " << P0.x << ", " << P0.y << std::endl;

    // Calculate the intermediate points
    // Change NUM_POINTS to change the quality of the curve
    const int NUM_POINTS = 100;

    m_vertices = new float[NUM_POINTS * 2];
    for (int i = 0; i < NUM_POINTS; i++) {
        float t = (float)i / (float)(NUM_POINTS-1);
        m_vertices[i * 2] = (1 - t) * (1 - t) * (1 - t) * P0.x + 3 * (1 - t) * (1 - t) * t * P1.x + 3 * (1 - t) * t * t * P2.x + t * t * t * P3.x;
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glEnableVertexAttribArray(0);

    m_IsUpdated = true;

}

void BezierCurve::Draw()
{
    m_Shader->SetVec3("lineColor", m_Color);
    m_rotationMatrixInversed = glm::mat2(1.0f);
    m_Shader->SetMat3("rotationMatrixInversed", m_rotationMatrixInversed);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINE_STRIP, 0, 100);
    glBindVertexArray(0);
}
