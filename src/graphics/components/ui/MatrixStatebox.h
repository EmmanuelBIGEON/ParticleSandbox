#pragma once

#include <map>

#include "../../main/GraphicContext.h"

class Statebox;

//! \class MatrixStatebox
//! \brief This class is used to draw the matrix state box on screen of specific size.
class MatrixStatebox
{
    public:
        MatrixStatebox(int m_dimSize);
        ~MatrixStatebox();

        void Init(GraphicContext* context);

        Statebox* GetStatebox(int x, int y);

        void PrintDim();
        int GetDim();

    private:

        int m_dimSize;

        std::map<std::pair<int, int>, Statebox*> m_stateboxes;
};