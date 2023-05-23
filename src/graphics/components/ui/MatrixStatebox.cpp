#pragma once

#include "MatrixStatebox.h"
#include "Statebox.h"

#include <glm/ext.hpp>

float MatrixStatebox::padding = 5.0f;

MatrixStatebox::MatrixStatebox(int dimSize, const glm::vec2& position) : m_dimSize(dimSize), m_position(position)
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
    float posX,posY = 0.0f;
    // Create the stateboxes.
    for(int i = 0; i < m_dimSize; ++i)
    {
            posY = m_position.y + (padding + sizeState) * (m_dimSize - (i+1)) + padding;
        for(int j = 0; j < m_dimSize; ++j)
        {
            posX = m_position.x + (padding + sizeState) * j + padding;
            Statebox* statebox = new Statebox(context, glm::vec2(posX, posY));
            m_stateboxes[std::make_pair(i, j)] = statebox;
            std::cout << "Created ! " << i << " " << j << std::endl;
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

float MatrixStatebox::GetWidth()
{
    return (m_dimSize * (padding + Statebox::sizeStatebox)) + padding;
}

float MatrixStatebox::GetHeight()
{
    return (m_dimSize * (padding + Statebox::sizeStatebox)) + padding;
}

void MatrixStatebox::PrintMap()
{
    // X Y ADRESS
    for(auto it = m_stateboxes.begin(); it != m_stateboxes.end(); ++it)
    {
        std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
    }
}

void MatrixStatebox::ToggleDisplay(bool displayed)
{
    for(auto it = m_stateboxes.begin(); it != m_stateboxes.end(); ++it)
    {
        it->second->SetToBeDisplayed(displayed);
    }
}