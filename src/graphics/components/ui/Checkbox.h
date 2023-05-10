#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include "../common/GraphicText.h"
#include "../common/GraphicImage.h"

#include "../../../common/Signal.h"

#include <glm/glm.hpp>
#include <string> 

//! \class Checkbox
//! \brief A Checkbox is a graphic object with a state.
class Checkbox : public GraphicObject
{
    public: 
        Checkbox(GraphicContext* context, bool* referencedValue, const glm::vec2& position, const glm::vec3& colorChecked = glm::vec3(0.0f,1.0f,1.0f));
        virtual ~Checkbox();

        virtual void Update() override;
        virtual void Draw() override;

        //! \brief Set the position of the Checkbox.
        void SetPosition(const glm::vec2& position);

        //! \brief Set the color of the Checkbox.
        void SetColorChecked(const glm::vec3& color);

        glm::vec2 GetPosition() const { return m_Pos; }
        glm::vec3 GetColorChecked() const { return m_ColorChecked; }

        void Connect(bool* referencedValue) { m_isChecked = referencedValue; }

        bool IsChecked() const { return m_isChecked; }

        bool m_isHovered;

        Signal<> OnClick;

        // Defined globally for all checkboxs
        static float sizeCheckbox;
    protected: 


        Shader* m_Shader;   

        glm::vec2 m_Pos;
        glm::vec3 m_ColorChecked;

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        float m_vertices[8];
        int m_indices[6];

        bool beingClicked;

        // Referenced bool
        bool* m_isChecked;
};