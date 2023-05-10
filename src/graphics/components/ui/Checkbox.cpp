#include "Checkbox.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "../../main/GraphicContext.h"

float Checkbox::sizeCheckbox = 18.0f;

Checkbox::Checkbox(GraphicContext* context, bool* referencedValue, const glm::vec2& position, const glm::vec3& colorChecked)
    : GraphicObject(context, SHADER_BUTTON), m_Pos(position), m_ColorChecked(colorChecked),  m_isHovered(false), beingClicked(false), m_isChecked(referencedValue),
    m_VAO(0), m_VBO(0), m_EBO(0)
{
    m_Shader = m_Context->GetShader(SHADER_BUTTON);

    // Connect to mouve move of the context

    m_Context->OnMouseMoved.Connect([this](float x, float y)
    {
        if(x > m_Pos.x && x < m_Pos.x + sizeCheckbox && 
           y > m_Pos.y && y < m_Pos.y + sizeCheckbox)
        {
            if(!m_isHovered)
            {
                m_isHovered = true;
            }
        }
        else
        {
            m_isHovered = false;
        }
    });

    m_Context->OnMousePressed.Connect([this](float x, float y)
    {
        if(x > m_Pos.x && x < m_Pos.x + sizeCheckbox && 
           y > m_Pos.y && y < m_Pos.y + sizeCheckbox)
        {
            OnClick.Emit();
            beingClicked = true;
            *m_isChecked = !*m_isChecked;
            m_Context->OnMousePressed.PreventDefault();
        }   
    });

    m_Context->OnMouseReleased.Connect([this](float x, float y)
    {
        if(beingClicked)
        {
            m_Context->OnMouseReleased.PreventDefault();
            beingClicked = false;
        }
    });
}

Checkbox::~Checkbox()
{
}

void Checkbox::Update()
{
    if(m_VAO == 0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
    }
    
    // Default form Checkbox is a rectangle
    // x                                y
    m_vertices[0] = m_Pos.x;            m_vertices[1] = m_Pos.y; 
    m_vertices[2] = m_Pos.x + sizeCheckbox; m_vertices[3] = m_Pos.y;
    m_vertices[4] = m_Pos.x + sizeCheckbox; m_vertices[5] = m_Pos.y + sizeCheckbox;
    m_vertices[6] = m_Pos.x;            m_vertices[7] = m_Pos.y + sizeCheckbox;

    m_indices[0] = 0; m_indices[1] = 1; m_indices[2] = 2;
    m_indices[3] = 0; m_indices[4] = 2; m_indices[5] = 3;

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_IsUpdated = true;
}

void Checkbox::Draw()
{
    glBindVertexArray(m_VAO);
    if(*m_isChecked)
        m_Shader->SetVec3("buttonColor", m_ColorChecked);
    else
        m_Shader->SetVec3("buttonColor", glm::vec3(0.0f, 0.2f, 0.35f));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
}

void Checkbox::SetPosition(const glm::vec2& position)
{
    m_Pos = position;
    m_IsUpdated = false;
}

void Checkbox::SetColorChecked(const glm::vec3& color)
{
    m_ColorChecked = color;
    m_IsUpdated = false;
}