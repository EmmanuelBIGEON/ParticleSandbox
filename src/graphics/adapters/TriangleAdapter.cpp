#include "TriangleAdapter.h"

#include <glad/glad.h>
#include <iostream>

TriangleAdapter::TriangleAdapter(GraphicContext* context, const Triangle& triangle) : GraphicObject(context,SHADER_BASIC), 
    m_Triangle(triangle), m_VAO(0), m_VBO(0), m_Color(1.0f, 0.0f, 1.0f), m_Alpha(1.0f)
{
}

TriangleAdapter::~TriangleAdapter()
{
}

void TriangleAdapter::Update()
{
    // float vertices[] = {
    //     m_Triangle.GetP1().x, m_Triangle.GetP1().y, 1.0f,   m_Color.x, m_Color.y, m_Color.z, m_Alpha,
    //     m_Triangle.GetP2().x, m_Triangle.GetP2().y, 1.0f,   m_Color.x, m_Color.y, m_Color.z, m_Alpha,
    //     m_Triangle.GetP3().x, m_Triangle.GetP3().y, 1.0f,   m_Color.x, m_Color.y, m_Color.z, m_Alpha,
    // };

    m_vertices[0] = m_Triangle.GetP1().x;m_vertices[1] = m_Triangle.GetP1().y;
    m_vertices[2] = 1.0f;m_vertices[3] = m_Color.x;
    m_vertices[4] = m_Color.y;m_vertices[5] = m_Color.z;
    m_vertices[6] = m_Alpha;

    m_vertices[7] = m_Triangle.GetP2().x;m_vertices[8] = m_Triangle.GetP2().y;
    m_vertices[9] = 1.0f;m_vertices[10] = m_Color.x;
    m_vertices[11] = m_Color.y;m_vertices[12] = m_Color.z;
    m_vertices[13] = m_Alpha;

    m_vertices[14] = m_Triangle.GetP3().x;m_vertices[15] = m_Triangle.GetP3().y;
    m_vertices[16] = 1.0f;m_vertices[17] = m_Color.x;
    m_vertices[18] = m_Color.y;m_vertices[19] = m_Color.z;
    m_vertices[20] = m_Alpha;


    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);

    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    glBindVertexArray(0);

    m_IsUpdated = true;
}

void TriangleAdapter::Draw()
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
