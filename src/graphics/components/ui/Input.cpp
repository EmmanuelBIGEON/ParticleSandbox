#include "Input.h"

#include "Button.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "../../main/GraphicContext.h"
#include "../../components/adapters/RectangleAdapter.h"

float Input::height = 45.0f;
float Input::width = 140.0f;

Input::Input(GraphicContext* context, std::string placeHolder, float x, float y) :  GraphicObject(context, SHADER_UI),
    m_shiftX(x), m_shiftY(y), m_EBO(0), m_VAO(0), m_VBO(0), isHovered(false), isFocused(false), m_numberOnly(false),
    m_VAO_cursor(0), m_VBO_cursor(0), m_EBO_cursor(0), m_value(placeHolder), m_maxSize(0)
{
    m_Shader = m_Context->GetShader(SHADER_UI);

    float xtext = x - (width / 2.0f);
    float ytext = y - (height / 2.0f);

    float left = m_shiftX - (width / 2.0f);
    float right = m_shiftX + (width / 2.0f);
    float top = m_shiftY + (height / 2.0f);
    float bottom = m_shiftY - (height / 2.0f);
    
    m_ValueTextObject = new GraphicText(m_Context, m_value.c_str(), glm::vec2(xtext, ytext + height), glm::vec2(xtext + width, ytext));
    m_ValueTextObject->SetAlignment(ALIGN_LEFT);
    m_ValueTextObject->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
    m_Context->OnKeyPressed.Connect([this](char key) {
        if(isFocused)
        {
            std::string text = m_ValueTextObject->GetText();
            if(key == 3)
            {
                text = text.substr(0, text.size() - 1);
            }else
            {
                if(m_numberOnly)
                {
                    if(key < 48 || key > 57)
                    {
                        return;
                    }
                }
                // m_maxSize
                if(m_maxSize)
                {
                    if(text.size() >= m_maxSize)
                    {
                        return;
                    }
                }
                text += key;
            }
            m_ValueTextObject->SetText(text.c_str());
        } 
    });

    m_Context->OnMouseMoved.Connect([this, left, right, top, bottom](float x, float y)
    {
        if(x > left && x < right 
        && y > bottom && y < top)
        {
            isHovered = true;
        }
        else
        {
            isHovered = false;
        }
    });

    m_Context->OnMousePressed.Connect([this](float x, float y)
    {
        if(isHovered)
        {
            isFocused = true;
        }
        else
        {
            isFocused = false;
        }
    });
}

Input::~Input()
{
}

void Input::Update()
{
    if(m_VAO == 0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
    }
    
    // x                                y
    m_vertices[0] = (-width / 2.0f); // Corner
    m_vertices[1] = (-height / 2.0f); // bottom left
    m_vertices[2] = (-width / 2.0f); // Corner
    m_vertices[3] = (height / 2.0f); // top left
    m_vertices[4] = (width / 2.0f); // Corner
    m_vertices[5] = (height / 2.0f); // top right
    m_vertices[6] = (width / 2.0f); // Corner
    m_vertices[7] = (-height / 2.0f); // bottom right

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_IsUpdated = true;
}

void Input::Draw()
{
    glBindVertexArray(m_VAO);
    m_Shader->SetVec3("elementColor", glm::vec3(0.10f, 0.10f, 0.0f));
    m_Shader->SetVec2("shiftPos", glm::vec2(m_shiftX, m_shiftY));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
}