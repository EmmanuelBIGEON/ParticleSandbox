#include "ParticleMatrix.h"

#include "MatrixStatebox.h"
#include "Statebox.h"

#include "../adapters/ParticleAdapter.h"


ParticleMatrix::ParticleMatrix(const glm::vec2& position) : m_matStatebox(nullptr), m_position(position)
{
}

ParticleMatrix::~ParticleMatrix()
{
}

void ParticleMatrix::Init(GraphicContext* context)
{
    const std::vector<ParticleClass>& vec = context->GetParticlesClasses();
    int dim = vec.size();

    if(m_matStatebox)
    {
        delete m_matStatebox;
        m_matStatebox = nullptr;
    }

    // Shouldnt happen, but for safety, clean first.
    if(m_particles.size() != 0)
    {
        for(auto it = m_particles.begin(); it != m_particles.end(); ++it) delete *it;
        m_particles.clear();
    }

    // Create Matrix.
    m_matStatebox = new MatrixStatebox(dim, m_position);
    m_matStatebox->Init(context);

    float padding = MatrixStatebox::padding;
    float sizeParticle = Statebox::sizeStatebox / 2.0f;

    int stateid = 0;
    // Add Repulsion behavior to all
    for(int i = 0; i < dim; ++i)
    {
        for(int j = 0; j < dim; ++j)
        {
            // retrieve behavior from context.
            ParticleBehavior* behavior = context->GetParticleBehavior(vec[i], vec[j]);
            if(!behavior)
            {
                std::cout << "No behavior found for " << vec[i] << " and " << vec[j] << std::endl;
                continue;
            }

            // Add behavior to statebox.
            Statebox* statebox = m_matStatebox->GetStatebox(i, j);
            if(!statebox)
            {
                std::cout << "No statebox found for " << i << " and " << j << std::endl;
                continue;
            }
            // stateid = statebox->AddState(glm::vec3(1.0f, 0.0f, 0.0f));
            // statebox->AddStateAction(stateid, [i,j](){std::cout << "Repulsion " << i << " " << j << std::endl;});

            stateid = statebox->AddState(glm::vec3(1.0f, 0.0f, 0.0f));
            statebox->AddStateAction(stateid, [behavior](){behavior->Repulsion();});
            stateid = statebox->AddState(glm::vec3(1.0f, 1.0f, 1.0f));
            statebox->AddStateAction(stateid, [behavior](){behavior->None();});
            stateid = statebox->AddState(glm::vec3(0.0f, 1.0f, 0.0f));
            statebox->AddStateAction(stateid, [behavior](){behavior->Attraction();});

            // Use manual update for the setup of behaviors.
            // statebox->ManualTrigger();
        }

        
        // draw the particles on top and left of their column and lines respectively.
        float posX_column = m_position.x + (padding + Statebox::sizeStatebox) * i + padding + sizeParticle;
        float posY_column = m_position.y + m_matStatebox->GetHeight() + sizeParticle;
        float posX_line = m_position.x - (padding + Statebox::sizeStatebox) + sizeParticle + padding;
        float posY_line = m_position.y + m_matStatebox->GetHeight() - (padding + Statebox::sizeStatebox) * i - padding - sizeParticle;
        ParticleAdapter* particle_col = new ParticleAdapter(context, glm::vec2(posX_column, posY_column), 
            context->GetColorParticle(vec[i]), sizeParticle);

        ParticleAdapter* particle_line = new ParticleAdapter(context, glm::vec2(posX_line, posY_line),
            context->GetColorParticle(vec[i]), sizeParticle);
            
        m_particles.push_back(particle_col);
        m_particles.push_back(particle_line);

    }
}

void ParticleMatrix::Update()
{
}
