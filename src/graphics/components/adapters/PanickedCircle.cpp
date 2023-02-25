#include "PanickedCircle.h"

#include <glad/glad.h>
#include <iostream>

PanickedCircle::PanickedCircle(GraphicContext* context, const Circle& circle) 
: GraphicObject(context, SHADER_BASIC), m_Circle(circle), m_vertices(nullptr), m_nbVertices(0), m_VAO(0), m_VBO(0), m_Color(glm::vec3(0.0f, 1.0f, 0.0f)), needRecompute(true)
{
    m_Shader = context->GetShader(SHADER_BASIC);
}

PanickedCircle::~PanickedCircle()
{
    if(m_vertices != nullptr)
    {
        m_nbVertices = 0;
        delete[] m_vertices;
    }
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void PanickedCircle::Update()
{
    // security
    if(m_Circle.GetRadius() <= 0.0f)
        return;

    float center_x = m_Circle.GetCenter().x;
    float center_y = m_Circle.GetCenter().y;

    if(!needRecompute)
    {
        // We move the center of the circle
        // maximum distance is 5.0f
        // velocity is 0.1f
        float distance = 25.0f;
        float velocity = 0.1f;
        float x = (rand() % (int)(distance * 2.0f)) - distance;
        float y = (rand() % (int)(distance * 2.0f)) - distance;
        m_shiftPos = glm::translate(glm::mat4(1.0f), glm::vec3(x * velocity, y * velocity, 0.0f));
        // IMPORTANT RETURN ! Notice !
        return;
    }else 
    {
        needRecompute = false;
    }
    
    if(m_vertices != nullptr)
    {
        m_nbVertices = 0;
        delete[] m_vertices;

        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);

    }
        
    // Calculate the number of vertices depending on the radius (for a better circle)
    // More vertices as the circle is bigger.
    // using Trigonometry cos(theAngleWeWant) because we know the radius (hypotenuse) and the adjacent side (hypotenuse - precision)
    float radius = m_Circle.GetRadius();
    float angle = glm::degrees(acos((radius - 1.0f) / radius));
    m_nbVertices = (int)(360.0f / angle);

    // Allocate memory for the vertices
    // Each vertex has 7 floats (x, y, z, r, g, b, a)
    m_vertices = new float[m_nbVertices * 7];

    for(int i = 0; i < m_nbVertices; i++)
    {
        // Calculate the angle in radians
        float rad = glm::radians(angle * i);

        // Calculate the x and y position
        float x = center_x + cos(rad) * radius;
        float y = center_y + sin(rad) * radius;

        // Set the position
        m_vertices[i*7 + 0] = x;
        m_vertices[i*7 + 1] = y;
        m_vertices[i*7 + 2] = 1.0f;

        // Set the color
        m_vertices[i*7 + 3] = m_Color.r;
        m_vertices[i*7 + 4] = m_Color.g;
        m_vertices[i*7 + 5] = m_Color.b;
        m_vertices[i*7 + 6] = 1.0f;
    }

    // opengl
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_nbVertices * 7 * sizeof(float), m_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);

    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Logically this isn't necessary if the application is well written
    // need a benchmark if it matters with bigger context.
    glBindVertexArray(0);

    // Perpetually update the circle to make it wiggle in panick !
    // It is covering in fear !
    // m_IsUpdated = true;
}

void PanickedCircle::Draw()
{
    
    m_Shader->SetMat4("shiftPos", m_shiftPos);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_nbVertices);
    m_Shader->SetMat4("shiftPos", glm::mat4(1.0f));
    glBindVertexArray(0);
}
