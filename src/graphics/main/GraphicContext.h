#pragma once

#include <vector>

#include <glm/ext.hpp>


#include "../../common/Signal.h"

class ParticleAdapter;

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
    SHADER_PARTICLE
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

        //! Zoom in the view.
        void ZoomIn(float factor);
        //! Zoom out the view.
        void ZoomOut(float factor);

        // For TextRendering
        Shader* GetShader(AvailableShader shader);

        const std::vector<GraphicObject*> GetObjects() const { return m_Objects; }
        const std::vector<ParticleAdapter*> GetParticleAdapters() const { return m_ParticleAdapters; }

        Font* font_main;

        static float worldWidth;
        static float worldHeight;

        Signal<GraphicObject*> OnObjectRegistered;
        Signal<GraphicObject*> OnObjectRemoved;

    protected:

        //! Render particles in multiple threads
        virtual void RenderThread(int nbOfThreads, int threadId);
        
        bool okRendering;
        bool needUpdate;

        float zoomFactor;
        //! \brief List of all the objects to be rendered.
        //! This structure might be changed later.
        std::vector<GraphicObject*> m_Objects;

        // render separately the particles for efficiency
        std::vector<ParticleAdapter*> m_ParticleAdapters;

        Shader* shader_basic;
        Shader* shader_text;
        Shader* shader_texture;
        Shader* shader_lighting; // [won't be reshapped, kept for testing purposes]
        Shader* shader_line;
        Shader* shader_particle; // Principal shader for the simulation


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
        
};