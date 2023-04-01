#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include "../common/GraphicText.h"
#include "../common/GraphicImage.h"

#include "../../../common/Signal.h"

#include <glm/glm.hpp>
#include <string> 

//! \class Button
//! \brief A button is a graphic object that can be clicked to trigger an action.
class Button : public GraphicObject
{
    public: 
        Button(GraphicContext* context, const glm::vec2& position, const glm::vec2& size, 
            const glm::vec3& color = glm::vec3(0.2f,0.2f,0.2f), const std::string& text = "");
        virtual ~Button();

        virtual void Update() override;
        virtual void Draw() override;

        //! \brief Set the position of the button.
        void SetPosition(const glm::vec2& position);

        //! \brief Set the size of the button.
        void SetSize(const glm::vec2& size);

        //! \brief Set the color of the button.
        void SetColor(const glm::vec3& color);

        //! \brief Set pathicon of the button.
        // Create a graphicImage that will replace the text of the button
        void SetPathIcon(const std::string& pathicon);
        void SetIconWidth(float width);
        void SetIconHeight(float height);
        

        //! \brief Set the text of the button.
        void SetText(const std::string& text);

        //! \brief Set the hovering color of the button.
        void SetHoveringColor(const glm::vec3& color);

        void SetActive(bool active);
        void SetActiveColor(const glm::vec3& color);
        void SetInvertIconOnActive(bool invertIconOnActive);

        glm::vec2 GetPosition() const { return m_Pos; }
        glm::vec2 GetSize() const { return m_Size; }
        glm::vec3 GetColor() const { return m_Color; }
        glm::vec3 GetHoveringColor() const { return m_HoveringColor; }
        std::string GetText() const { return m_Text; }

        bool m_isHovered;

        Signal<> OnClick;

    protected: 

        Shader* m_Shader;   
        glm::vec2 m_Pos;
        glm::vec2 m_Size;
        glm::vec3 m_Color;
        glm::vec3 m_HoveringColor;
        glm::vec3 m_ActiveColor;
        std::string m_Text;


        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        float m_vertices[8];
        int m_indices[6];

        GraphicText* m_TextObject;
        GraphicImage* m_ImageObject;

        bool m_isActive;
        bool m_InvertIconOnActive;

        bool beingClicked;
};