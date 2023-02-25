#include "RectangleAdapter.h"

#include <glad/glad.h>


RectangleAdapter::RectangleAdapter(GraphicContext* context, const Rectangle& rectangle) : GraphicObject(context, SHADER_BASIC),
 m_Rectangle(rectangle), m_Color(glm::vec3(0.0f, 1.0f, 1.0f)), m_Alpha(1.0f), m_VAO(0), m_VBO(0), m_EBO(0)
{
}

 RectangleAdapter::~RectangleAdapter()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void RectangleAdapter::SetColor(const glm::vec3& color)
{
    m_Color = color;
}

void RectangleAdapter::SetOpacity(float alpha)
{
    m_Alpha = alpha;
}

void RectangleAdapter::Update()
{
    // quick calculations
    int width = m_Rectangle.GetP2().x - m_Rectangle.GetP1().x;
    int height = m_Rectangle.GetP2().y - m_Rectangle.GetP1().y;

    m_vertices[0] = m_Rectangle.GetP1().x;
    m_vertices[1] = m_Rectangle.GetP1().y;
    m_vertices[2] = 0.0f;
    m_vertices[3] = m_Color.r;m_vertices[4] = m_Color.g;m_vertices[5] = m_Color.b;
    m_vertices[6] = m_Alpha;

    m_vertices[7] = m_Rectangle.GetP1().x;
    m_vertices[8] = m_Rectangle.GetP1().y + height;
    m_vertices[9] = 0.0f;
    m_vertices[10] = m_Color.r;m_vertices[11] = m_Color.g;m_vertices[12] = m_Color.b;
    m_vertices[13] = m_Alpha;

    m_vertices[14] = m_Rectangle.GetP1().x + width;
    m_vertices[15] = m_Rectangle.GetP1().y + height;
    m_vertices[16] = 0.0f;
    m_vertices[17] = m_Color.r;m_vertices[18] = m_Color.g;m_vertices[19] = m_Color.b;
    m_vertices[20] = m_Alpha;

    m_vertices[21] = m_Rectangle.GetP1().x + width;
    m_vertices[22] = m_Rectangle.GetP1().y;
    m_vertices[23] = 0.0f;
    m_vertices[24] = m_Color.r;m_vertices[25] = m_Color.g;m_vertices[26] = m_Color.b;
    m_vertices[27] = m_Alpha;

    // opengl

    // VAO  
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // VBO data
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    // EBO data
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // VAO attributes   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_IsUpdated = true;
}

void RectangleAdapter::Draw()
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

