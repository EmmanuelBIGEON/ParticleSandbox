#pragma once

#include <vector>

class GraphicObject;
class Shader;

enum AvailableShader
{
    SHADER_BASIC = 0
};

//! \class GraphicContext
class GraphicContext
{
    public: 
        GraphicContext();
        virtual ~GraphicContext();

        virtual void Init();
        
        //! \brief Main rendering function.
        //! It automacally calls the Update() method of the object.
        virtual void Render();

        //! \brief Register a graphic object to the context for it to be rendered.
        virtual void Register(GraphicObject* object);
        
        virtual void Update(); // placeholder function for now, might be used later

    protected:
        bool okRendering;

        //! \brief List of all the objects to be rendered.
        //! This structure might be changed later.
        std::vector<GraphicObject*> m_Objects;

        Shader* shader_basic;
};