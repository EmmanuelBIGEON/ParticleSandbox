#pragma once

#include <vector>

#include <glm/ext.hpp>

class GraphicObject;
class Shader;
class Font;

enum AvailableShader
{
    SHADER_BASIC = 0,
    SHADER_TEXT
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


        
        //! \brief Main rendering function.
        //! It automacally calls the Update() method of the object.
        virtual void Render();

        //! \brief Register a graphic object to the context for it to be rendered.
        virtual void Register(GraphicObject* object);
        
        virtual void Update(); // placeholder function for now, might be used later

        // For TextRendering
        Shader* GetShader(AvailableShader shader);

    protected:
        bool okRendering;
        bool needUpdate;

        //! \brief List of all the objects to be rendered.
        //! This structure might be changed later.
        std::vector<GraphicObject*> m_Objects;

        Shader* shader_basic;
        Shader* shader_text;

        Font* font_main;

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
        
};