#pragma once

#include <vector>
#include <mutex>
#include <map>
#include <glm/ext.hpp>

#include "../../common/Signal.h"
#include "ParticleImpl.h"

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

        //! \brief Pause the rendering of particles.
        void Pause() { m_Pause = true; }

        //! \brief Resume the rendering of particles.
        void Resume() { m_Pause = false; }

        //! \brief Add a vector of particles2 and updates the useful data.
        void AddParticles(int nbParticles, ParticleClass particleClass = ParticleClass::PART_CLASS_1, 
            float rangex_min = 0.0f, float rangex_max = GraphicContext::worldWidth, float rangey_min = 0.0f, float rangey_max = GraphicContext::worldHeight);
        void AddParticles(const std::vector<ParticleStruct>& particles, ParticleClass particleClass = ParticleClass::PART_CLASS_1);

        void SetColor_particles_type1(const glm::vec3& color) { PA1_color = color; }
        void SetColor_particles_type2(const glm::vec3& color) { PA2_color = color; }
        void SetColor_particles_type3(const glm::vec3& color) { PA3_color = color; }

        // For TextRendering
        Shader* GetShader(AvailableShader shader);

        const std::vector<GraphicObject*> GetObjects() const { return m_Objects; }

        const int& GetNbParticles_type1() const { return m_nb_PA1; }
        const int& GetNbParticles_type2() const { return m_nb_PA2; }
        const int& GetNbParticles_type3() const { return m_nb_PA3; }

        Font* font_main;
        

        
        static float worldWidth;
        static float worldHeight;
        static float repulsion_factor;
        static float attraction_factor;
        static float repulsion_maximum_distance;
        static float attraction_threshold_distance;
        static glm::vec3 PA1_color;
        static glm::vec3 PA2_color;
        static glm::vec3 PA3_color;
        static bool useVelocity;
        static bool behaviorDriven;

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

        //! \brief Signal emitted when a key is pressed.
        //! Allowing UI elements to connect to it.
        Signal<char> OnKeyPressed;


        //! \brief Signal when a graphic object join or leave the context.
        Signal<GraphicObject*> OnObjectRegistered;
        Signal<GraphicObject*> OnObjectRemoved;
        
        //! \brief Signal added particles to the context.
        Signal<int> OnParticlesAdded;


    protected:

        //! Main function for rendering Particle, take particles of same class, and other class.
        // For now, act the same way for all the particles, but later, each class will have a behavior depending on the targeted class.
        void RenderParticles(ParticleClass particleClass);

        // incredibly slow method but, allow the program to run on all machines (using multithreading)
        void RenderParticles_without_avx(ParticleClass particleClass);

        // Works with RenderParticles_without_avx
        // Allow to render particles in a multithreaded way
        void ComputeParticles_thread(ParticleClass particleClass, int start, int end);

        // Simply draw the particles
        void DrawParticles(ParticleClass particleClass);

        // Instanciate behavior of particles
        void InitBehaviors();
        
        bool okRendering;
        bool needUpdate;

        float zoomFactor;

        //! \brief List of all the objects to be rendered.
        //! This structure might be changed later.
        std::vector<GraphicObject*> m_Objects;

        // Using SIMD operations to render particles
        // Need to split attributes in 3 arrays
        float* m_PA1_posX;
        float* m_PA1_posY;
        float* m_PA1_mass; // not used, maybe later
        float* m_PA1_velocityX;
        float* m_PA1_velocityY;
        int m_nb_PA1;

        float* m_PA2_posX;
        float* m_PA2_posY;
        float* m_PA2_mass; // not used, maybe later
        float* m_PA2_velocityX;
        float* m_PA2_velocityY;
        int m_nb_PA2;

        float* m_PA3_posX;
        float* m_PA3_posY;
        float* m_PA3_mass; // not used, maybe later
        float* m_PA3_velocityX;
        float* m_PA3_velocityY;
        int m_nb_PA3;

        // List of all the particle classes 
        // used for calculation.
        std::vector<ParticleClass> m_ParticleClasses;

        // List behaviour for each particle class, the left one is the source and the behavior affect how the right one affect the left one.
        // For instance, to give a behavior for particleClass1, define (PART_CLASS_1, PART_CLASS_1) and (PART_CLASS_1, PART_CLASS_2) and (PART_CLASS_1, PART_CLASS_3)
        std::map<std::pair<ParticleClass, ParticleClass>, ParticleBehaviour> m_ParticleBehaviours;

        // mutex for the particle adapters
        std::mutex m_ParticleAdaptersMutex;

        Shader* shader_basic;
        Shader* shader_text;
        Shader* shader_texture;
        Shader* shader_lighting; // [won't be reshapped, kept for testing purposes]
        Shader* shader_line;
        Shader* shader_particle; // Main shader for the simulation
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

        bool m_Pause;
};