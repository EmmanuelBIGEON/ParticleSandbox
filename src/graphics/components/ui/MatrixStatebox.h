#pragma once

#include <map>

#include "../../main/GraphicContext.h"

#include <glm/ext.hpp>

class Statebox;

//! \class MatrixStatebox
//! \brief This class is used to draw the matrix state box on screen of specific size.
class MatrixStatebox
{
    public:
        MatrixStatebox(int m_dimSize, const glm::vec2& position);
        ~MatrixStatebox();

        void Init(GraphicContext* context);

        Statebox* GetStatebox(int x, int y);

        void PrintDim();
        void PrintMap();

        int GetDim();
        float GetWidth();
        float GetHeight();

        void ToggleDisplay(bool displayed);

        static float padding;
    private:


        int m_dimSize;
        glm::vec2 m_position;

        std::map<std::pair<int, int>, Statebox*> m_stateboxes;
};