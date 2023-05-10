#pragma once

#include "../../main/GraphicContext.h"

#include "../adapters/ParticleAdapter.h"

#include <vector>

#include <glm/ext.hpp>

class MatrixStatebox;

//! \class ParticleMatrix
//! \brief This class allow quick changing of behavior between particles in a preset fashion.
//! Based on the particle of context.
class ParticleMatrix
{
    public:
        ParticleMatrix(const glm::vec2& position);
        ~ParticleMatrix();

        void Init(GraphicContext* context);
        void Update();

    private:

        MatrixStatebox* m_matStatebox;
        std::vector<ParticleAdapter*> m_particles;

        glm::vec2 m_position;
};