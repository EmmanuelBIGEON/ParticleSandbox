#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include <glm/ext.hpp>

#include <map>
#include <functional>

class Statebox : public GraphicObject
{
    public:
        Statebox(GraphicContext* context, const glm::vec2& position);
        ~Statebox();

        int AddState(const glm::vec3& color);

        virtual void Update() override;
        virtual void Draw() override;

        void ChangeState(int state);
        void ChangeState();

        // If state exist, replace/set a function to be called when the state is set to current state.
        void AddStateAction(int state, std::function<void()> action);

        // RETURN: the next state id.
        int NextState();

        //! Method used to trigger the action without waiting for the UI to receive a click.
        //! Launch current state action if there is one.
        //! Warning : Use this method only for special cases,  prioritize normal functionning.
        void ManualTrigger();

        
        // Defined globally for all stateboxs
        static float sizeStatebox;
    private:
    

        int current_state; 
        std::map<int, glm::vec3> states;
        std::map<int, std::function<void()>> states_actions;

        bool beingClicked;

        Shader* m_Shader;   
        glm::vec2 m_Pos;
        glm::vec3 m_Color;

        float m_vertices[8];
        int m_indices[6];
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
};