#pragma once

#include <vector>
#include <mutex>
#include <glm/ext.hpp>

#include "../../common/Signal.h"

class ParticleAdapter;
class ParticleStruct;

class GraphicObject;
class Shader;
class Font;

enum AvailableShader
{
    SHADER_BASIC = 0,
    SHADER_TEXT,
    SHADER_TEXTURE,
    SHADER_LIGHTING,
    SHADER_LINE,
    SHADER_PARTICLE,
    SHADER_BUTTON, // to remove..
    SHADER_UI
};

//! \class GraphicContext
class GraphicContext 
{
    public: 
        GraphicContext();
        virtual ~GraphicContext();

        virtual void Init();

        //! \brief Clear the context.
        virtual void Clear(bool DeleteObjects = false);

        //! \brief Update the model matrix.
        virtual void UpdateModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix = modelMatrix; needUpdate = true;}
        const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
        
        //! \brief Update the projection matrix.
        virtual void UpdateProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; needUpdate = true;}
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        
        //! \brief Update the view matrix.
        virtual void UpdateViewMatrix(const glm::mat4& viewMatrix) { m_ViewMatrix = viewMatrix; needUpdate = true;}
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

        float Convert_glX_to_WorldX(float glX) const;
        float Convert_glY_to_WorldY(float glY) const;
        
        //! \brief Main rendering function.
        //! It automacally calls the Update() method of the object.
        virtual void Render();

        //! \brief Register a graphic object to the context for it to be rendered.
        void Register(GraphicObject* object);

        //! \brief Remove a graphic object from the context.
        void Remove(GraphicObject* object);
        
        virtual void Update(); // placeholder function for now, might be used later

        //! \brief Add a vector of particles2 and updates the useful data.
        void AddParticles(int nbParticles);
        void AddParticles(const std::vector<ParticleStruct>& particles);

        //! Zoom in the view.
        void ZoomIn(float factor);
        //! Zoom out the view.
        void ZoomOut(float factor);

        // For TextRendering
        Shader* GetShader(AvailableShader shader);

        const std::vector<GraphicObject*> GetObjects() const { return m_Objects; }
        const std::vector<ParticleAdapter*> GetParticleAdapters() const { return m_ParticleAdapters; }

        const int& GetNbParticles() const { return nb_ParticleAdapters3; }

        Font* font_main;

        static float worldWidth;
        static float worldHeight;

        //! \brief Signal emitted when the mouse is moved.
        //! Allowing UI elements to connect to it.
        Signal<float, float> OnMouseMoved;

        //! \brief Signal emitted when the mouse is clicked.
        //! Allowing UI elements to connect to it.
        // Give the position (in world coordinates) of the mouse when it was clicked.
        Signal<float, float> OnMousePressed;

        //! \brief Signal emitted when the mouse is released.
        //! Allowing UI elements to connect to it.
        // Give the position (in world coordinates) of the mouse when it was released.
        Signal<float, float> OnMouseReleased;


        //! \brief Signal when a graphic object join or leave the context.
        Signal<GraphicObject*> OnObjectRegistered;
        Signal<GraphicObject*> OnObjectRemoved;
        
        //! \brief Signal added particles to the context.
        Signal<int> OnParticlesAdded;


    protected:

        //! Render particles in multiple threads
        virtual void RenderThread(int nbOfThreads, int threadId);
        
        bool okRendering;
        bool needUpdate;

        float zoomFactor;
        //! \brief List of all the objects to be rendered.
        //! This structure might be changed later.
        std::vector<GraphicObject*> m_Objects;

        //! subvector containing only hovered objects

        // render separately the particles for efficiency
        std::vector<ParticleAdapter*> m_ParticleAdapters;

        // ---- EXPERIMENTAL ----
        // Using SIMD operations to render particles
        // Need to split attributes in 3 arrays
        float* m_ParticleAdapters3_posX;
        float* m_ParticleAdapters3_posY;
        float* m_ParticleAdapters3_mass;
        int nb_ParticleAdapters3;
        // Maybe i can save the distance between the particles for even more efficiency. TODO

        Shader* shader_basic;
        Shader* shader_text;
        Shader* shader_texture;
        Shader* shader_lighting; // [won't be reshapped, kept for testing purposes]
        Shader* shader_line;
        Shader* shader_particle; // Principal shader for the simulation
        Shader* shader_button;
        Shader* shader_ui;


        //! \brief Projection matrix.
        //! Convert coordinates from world space to screen space.
        //! + (MaxY)-------Max
        //! | ---------------
        //! | ---------------
        //! | ---------------
        //! | ---------------
        //  O ______________+ (MaxX)
        glm::mat4 m_ModelMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;

        float m_ScreenWidth;
        float m_ScreenHeight;

        float m_MouseX;
        float m_MouseY;

        // mutex for the particle adapters
        std::mutex m_ParticleAdaptersMutex;
        
};