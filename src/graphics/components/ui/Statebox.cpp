#include "Statebox.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "../../main/Shader.h"

float Statebox::sizeStatebox = 180.0f;

Statebox::Statebox(GraphicContext* context, const glm::vec2& position) : GraphicObject(context, SHADER_BUTTON), m_Pos(position), 
    beingClicked(false), m_Color(0.0f, 0.0f, 0.0f), current_state(-1), VAO(0), VBO(0), EBO(0)
{
    m_Shader = m_Context->GetShader(SHADER_BUTTON);
    states.clear();
    states_actions.clear();
    
    m_Context->OnMousePressed.Connect([this](float x, float y)
    {
        if(x > m_Pos.x && x < m_Pos.x + sizeStatebox && 
           y > m_Pos.y && y < m_Pos.y + sizeStatebox)
        {
            beingClicked = true;
            ChangeState();

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

Statebox::~Statebox()
{
}

void Statebox::Update()
{
    if(VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }
    
    // Default form Checkbox is a rectangle
    // x                                y
    m_vertices[0] = m_Pos.x;            m_vertices[1] = m_Pos.y; 
    m_vertices[2] = m_Pos.x + sizeStatebox; m_vertices[3] = m_Pos.y;
    m_vertices[4] = m_Pos.x + sizeStatebox; m_vertices[5] = m_Pos.y + sizeStatebox;
    m_vertices[6] = m_Pos.x;            m_vertices[7] = m_Pos.y + sizeStatebox;

    m_indices[0] = 0; m_indices[1] = 1; m_indices[2] = 2;
    m_indices[3] = 0; m_indices[4] = 2; m_indices[5] = 3;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_IsUpdated = true;
}

void Statebox::Draw()
{
    if(current_state == -1) 
        return;

    glBindVertexArray(VAO);
    m_Shader->SetVec3("buttonColor", m_Color);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


int Statebox::AddState(const glm::vec3& color)
{
    if(states.size() == 0) 
    {
        current_state = 0;
        m_Color = color;
    }
    int state = states.size();
    states[state] = color;
    return state;
}

void Statebox::ChangeState()
{
    int state = NextState();
    if(state == -1) return;

    current_state = state;

    // trigger the action
    if(states_actions.find(state) != states_actions.end())
        states_actions[current_state]();

    m_Color = states[state];
    
}

void Statebox::ChangeState(int state)
{
    if(state == -1) return;

    current_state = state;

    // trigger the action
    if(states_actions.find(state) != states_actions.end())
        states_actions[state]();
    
    m_Color = states[state];
}

void Statebox::AddStateAction(int state, std::function<void()> action)
{
    states_actions[state] = action;
}

int Statebox::NextState()
{
    bool found = false;
    // in case the current is the last one, send the first one.
    bool first = true;
    int res = -1;

    for(const auto &elem : states)
    {
        if(first)
        {
            first = false;
            res = elem.first;
        }

        if(found){
            res = elem.first;
            break;
        }
        
        // setup for the next iteration
        if(elem.first == current_state) found = true;
    }

    return res;
}
