#include "Button.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "../../main/GraphicContext.h"

Button::Button(GraphicContext* context, const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const std::string& text)
    : GraphicObject(context, SHADER_BUTTON), m_Pos(position), m_Size(size), m_Color(color), m_Text(text), m_isHovered(false), beingClicked(false),
    m_VAO(0), m_VBO(0), m_EBO(0), m_TextObject(nullptr), m_isActive(false), m_ImageObject(nullptr), m_ActiveColor(color), m_InvertIconOnActive(false),
    display(true), m_closeRangeButton(false), m_rangeButtonDistance(200.0f), m_onMouseMovedSlot(nullptr), m_onMousePressedSlot(nullptr), m_onMouseReleasedSlot(nullptr)
{
    m_Shader = m_Context->GetShader(SHADER_BUTTON);

    // default calculation of m_HoveringColor
    // if the color is too dark, the hovering color will be lighter
    // if the color is too light, the hovering color will be darker
    float sum = m_Color.r + m_Color.g + m_Color.b;
    if(sum < 1.5f) 
        m_HoveringColor = m_Color + glm::vec3(0.3f, 0.3f, 0.3f);
    else
        m_HoveringColor = m_Color - glm::vec3(0.3f, 0.3f, 0.3f);

    int textX = m_Pos.x + m_Size.x / 2 - m_Text.length() * 10;
    int textY = m_Pos.y + m_Size.y / 2 - m_Context->font_main->GetSize() / 2;
    
    m_TextObject = new GraphicText(m_Context, m_Text.c_str(), glm::vec2(m_Pos.x, m_Pos.y + m_Size.y), glm::vec2(m_Pos.x + m_Size.x, m_Pos.y));
    m_TextObject->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));

    float image_posx = m_Pos.x + m_Size.x / 2 - 12.5f;
    float image_posy = m_Pos.y + m_Size.y / 2 - 12.5f;
    m_ImageObject = new GraphicImage(m_Context, "", image_posx, image_posy, 25.0f, 25.0f);

    float rangeposx = m_Pos.x + m_Size.x / 2 - 12.5f;
    float rangeposy = m_Pos.y + m_Size.y / 2 - 12.5f;

    

    // Connect to mouve move of the context

    m_onMouseMovedSlot = m_Context->OnMouseMoved.Connect([this, rangeposx, rangeposy](float x, float y)
    {
        if(x > m_Pos.x && x < m_Pos.x + m_Size.x && 
           y > m_Pos.y && y < m_Pos.y + m_Size.y)
        {
            if(!m_isHovered)
            {
                m_isHovered = true;
                if(m_TextObject)
                    m_TextObject->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
        else
        {
            if(m_isHovered)
            {
                m_isHovered = false;
                if(m_TextObject)
                    m_TextObject->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }

        if(m_closeRangeButton)
        {
            float distance = sqrt(pow(x - rangeposx, 2) + pow(y - rangeposy, 2));
            if(distance > m_rangeButtonDistance)
            {
                display = false;
                m_ImageObject->SetToBeDisplayed(false);
                m_TextObject->SetToBeDisplayed(false);
            }
            else
            {
                display = true;
                m_ImageObject->SetToBeDisplayed(true);
                m_TextObject->SetToBeDisplayed(true);
            }
        }
    });

    m_onMousePressedSlot = m_Context->OnMousePressed.Connect([this](float x, float y)
    {
        if(x > m_Pos.x && x < m_Pos.x + m_Size.x && 
           y > m_Pos.y && y < m_Pos.y + m_Size.y)
        {
            OnClick.Emit();
            beingClicked = true;
            m_Context->OnMousePressed.PreventDefault();
        }   
    });

    m_onMouseReleasedSlot = m_Context->OnMouseReleased.Connect([this](float x, float y)
    {
        if(beingClicked)
        {
            m_Context->OnMouseReleased.PreventDefault();
            beingClicked = false;
        }
    });
}

Button::~Button()
{
}

void Button::Update()
{
    if(m_VAO == 0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
    }
    
    // Default form button is a rectangle
    // x                                y
    m_vertices[0] = m_Pos.x;            m_vertices[1] = m_Pos.y; 
    m_vertices[2] = m_Pos.x + m_Size.x; m_vertices[3] = m_Pos.y;
    m_vertices[4] = m_Pos.x + m_Size.x; m_vertices[5] = m_Pos.y + m_Size.y;
    m_vertices[6] = m_Pos.x;            m_vertices[7] = m_Pos.y + m_Size.y;

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

void Button::Draw()
{
    if(display)
    {
        glBindVertexArray(m_VAO);
        if(m_isActive)
        {
            m_Shader->SetVec3("buttonColor", m_ActiveColor);
        }else
        {
            if(m_isHovered)
                m_Shader->SetVec3("buttonColor", m_HoveringColor);
            else
                m_Shader->SetVec3("buttonColor", m_Color);
        }
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
        
}

void Button::SetPosition(const glm::vec2& position)
{
    m_Pos = position;
    m_IsUpdated = false;
}

void Button::SetSize(const glm::vec2& size)
{
    m_Size = size;
    m_IsUpdated = false;
}   

void Button::SetColor(const glm::vec3& color)
{
    m_Color = color;
    m_IsUpdated = false;
}

void Button::SetText(const std::string& text)
{
    m_Text = text;
    m_TextObject->SetText(text);
    m_IsUpdated = false;
}

void Button::SetHoveringColor(const glm::vec3& color)
{
    m_HoveringColor = color;
    m_IsUpdated = false;
}

void Button::SetPathIcon(const std::string& pathicon)
{
    m_ImageObject->SetImage(pathicon);
}

void Button::SetIconWidth(float width)
{
    m_ImageObject->SetWidth(width);
}

void Button::SetIconHeight(float height)
{
    m_ImageObject->SetHeight(height);
}

void Button::SetActive(bool active)
{
    m_isActive = active;
    if(m_ImageObject && m_InvertIconOnActive)
        m_ImageObject->SetToInvertColor(active);
}

void Button::SetActiveColor(const glm::vec3& color)
{
    m_ActiveColor = color;
}

void Button::SetInvertIconOnActive(bool invertIconOnActive)
{
    m_InvertIconOnActive = invertIconOnActive;
}

void Button::SetRangeButton(bool closeRangeButton)
{
    m_closeRangeButton = closeRangeButton;
    if(m_closeRangeButton)
    {
        m_ImageObject->SetToBeDisplayed(false);
        m_TextObject->SetToBeDisplayed(false);
        display = false;
    }else 
    {
        m_ImageObject->SetToBeDisplayed(true);
        m_TextObject->SetToBeDisplayed(true);
        display = true;
    }
}

void Button::SetRangeButtonDistance(float distance)
{
    m_rangeButtonDistance = distance;
}

void Button::SetToBeDisplayed(bool toBeDisplayed)
{
    m_IsToBeDisplayed = toBeDisplayed;

    if(m_IsToBeDisplayed)
    {
        m_onMouseMovedSlot->SetActive(true);
        m_onMousePressedSlot->SetActive(true);
        m_onMouseReleasedSlot->SetActive(true);
        if(m_TextObject) m_TextObject->SetToBeDisplayed(true);
        if(m_ImageObject) m_ImageObject->SetToBeDisplayed(true);
    }else 
    {
        m_onMouseMovedSlot->SetActive(false);
        m_onMousePressedSlot->SetActive(false);
        m_onMouseReleasedSlot->SetActive(false);
        if(m_TextObject) m_TextObject->SetToBeDisplayed(false);
        if(m_ImageObject) m_ImageObject->SetToBeDisplayed(false);
    }
    
}