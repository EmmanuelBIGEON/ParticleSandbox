#pragma once

#include "MatrixStatebox.h"
#include "Statebox.h"

#include <glm/ext.hpp>


MatrixStatebox::MatrixStatebox(int m_dimSize) : m_dimSize(m_dimSize)
{
}

MatrixStatebox::~MatrixStatebox()
{
}

void MatrixStatebox::Init(GraphicContext* context)
{
    if(m_stateboxes.size() != 0)
    {
        // DELETE
        for(auto it = m_stateboxes.begin(); it != m_stateboxes.end(); ++it)
        {
            delete it->second;
        }

        m_stateboxes.clear();
    }

    float sizeState = Statebox::sizeStatebox;
    float padding = 5.0f;
    float posX,posY = 0.0f;
    // Create the stateboxes.
    for(int i = 0; i < m_dimSize; ++i)
    {
        posX = (padding + sizeState) * i + padding;
        for(int j = 0; j < m_dimSize; ++j)
        {
            posY = (padding + sizeState) * j + padding;
            Statebox* statebox = new Statebox(context, glm::vec2(posX, posY));
            m_stateboxes[std::make_pair(i, j)] = statebox;
        }
    }
}

Statebox* MatrixStatebox::GetStatebox(int x, int y)
{
    // Check if the statebox exists.
    if(m_stateboxes.find(std::make_pair(x, y)) != m_stateboxes.end())
    {
        return m_stateboxes[std::make_pair(x, y)];
    }
    return nullptr;
}

void MatrixStatebox::PrintDim()
{
    std::cout << "DIM: " << m_dimSize << std::endl;
}

int MatrixStatebox::GetDim()
{
    return m_dimSize;
}
