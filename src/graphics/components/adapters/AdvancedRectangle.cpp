#include "AdvancedRectangle.h"

#include <glad/glad.h>

AdvancedRectangle::AdvancedRectangle(GraphicContext* context, const Rectangle& rectangle, const glm::vec3& color)
    : GraphicObject(context, SHADER_LIGHTING), m_Rectangle(rectangle), m_Color(color), m_VAO(0), m_VBO(0)
{
}

AdvancedRectangle::~AdvancedRectangle()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void AdvancedRectangle::Update()
{
    int width = m_Rectangle.GetP2().x - m_Rectangle.GetP1().x;
    int height = m_Rectangle.GetP2().y - m_Rectangle.GetP1().y;

    m_vertices[0]  =  m_Rectangle.GetP1().x; // x
    m_vertices[1]  =  m_Rectangle.GetP1().y; // y
    m_vertices[2]  =  0.0f; // z
    m_vertices[3]  =  0.0f; // normal x
    m_vertices[4]  =  0.0f; // normal y
    m_vertices[5]  = -1.0f; // normal z

    m_vertices[6]  =  m_Rectangle.GetP1().x; // x
    m_vertices[7]  =  m_Rectangle.GetP1().y + height; // y
    m_vertices[8]  =  0.0f; // z
    m_vertices[9]  =  0.0f; // normal x
    m_vertices[10] =  0.0f; // normal y
    m_vertices[11] = -1.0f; // normal z

    m_vertices[12] =  m_Rectangle.GetP1().x + width; // x
    m_vertices[13] =  m_Rectangle.GetP1().y + height; // y
    m_vertices[14] =  0.0f; // z
    m_vertices[15] =  0.0f; // normal x
    m_vertices[16] =  0.0f; // normal y
    m_vertices[17] =  -1.0f; // normal z

    m_vertices[18] =  m_Rectangle.GetP1().x + width; // x
    m_vertices[19] =  m_Rectangle.GetP1().y + height; // y
    m_vertices[20] =  0.0f; // z
    m_vertices[21] =  0.0f; // normal x
    m_vertices[22] =  0.0f; // normal y
    m_vertices[23] =  -1.0f; // normal z

    m_vertices[24] =  m_Rectangle.GetP1().x + width; // x
    m_vertices[25] =  m_Rectangle.GetP1().y; // y
    m_vertices[26] =  0.0f; // z
    m_vertices[27] =  0.0f; // normal x
    m_vertices[28] =  0.0f; // normal y
    m_vertices[29] = -1.0f; // normal z

    m_vertices[30] =  m_Rectangle.GetP1().x; // x
    m_vertices[31] =  m_Rectangle.GetP1().y; // y
    m_vertices[32] =  0.0f; // z
    m_vertices[33] =  0.0f; // normal x
    m_vertices[34] =  0.0f; // normal y
    m_vertices[35] = -1.0f; // normal z

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    m_IsUpdated = true;
}

void AdvancedRectangle::Draw()
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}
