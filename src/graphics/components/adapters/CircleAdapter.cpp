#include "CircleAdapter.h"

#include <glad/glad.h>
#include <iostream>

CircleAdapter::CircleAdapter(GraphicContext* context, const Circle& circle) 
: GraphicObject(context, SHADER_BASIC), m_Circle(circle), m_vertices(nullptr), m_nbVertices(0), m_VAO(0), m_VBO(0), m_Color(glm::vec3(0.0f, 1.0f, 0.0f))
{
}

CircleAdapter::~CircleAdapter()
{
    if(m_vertices != nullptr)
    {
        m_nbVertices = 0;
        delete[] m_vertices;
    }
}

void CircleAdapter::Update()
{
    if(m_vertices != nullptr)
    {
        m_nbVertices = 0;
        delete[] m_vertices;
    }

    // security
    if(m_Circle.GetRadius() <= 0.0f)
        return;
        
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
        float x = m_Circle.GetCenter().x + cos(rad) * radius;
        float y = m_Circle.GetCenter().y + sin(rad) * radius;

        // Set the position
        m_vertices[i*7 + 0] = x;
        m_vertices[i*7 + 1] = y;
        m_vertices[i*7 + 2] = 1.0f;

        // Set the color
        m_vertices[i*7 + 3] = m_Color.r;
        m_vertices[i*7 + 4] = m_Color.g;
        m_vertices[i*7 + 5] = m_Color.b;
        m_vertices[i*7 + 6] = 1.0f;
    std::cout << "CircleAdapter::Update() - i: " << i << std::endl;
    }

    std::cout << "CircleAdapter::Update() - m_nbVertices: " << m_nbVertices << std::endl;
    // opengl
    glGenVertexArrays(1, &m_VAO);
    std::cout << "CircleAdapter::glGenVertexArrays() - m_nbVertices: " << m_nbVertices << std::endl;
    glGenBuffers(1, &m_VBO);
    std::cout << "CircleAdapter::glGenBuffers() - m_nbVertices: " << m_nbVertices << std::endl;

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


    m_IsUpdated = true;
}

void CircleAdapter::Draw()
{
    // draw a filled circle
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_nbVertices);
    glBindVertexArray(0);
}
