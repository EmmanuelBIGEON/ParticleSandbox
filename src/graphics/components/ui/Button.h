#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include "../../../common/Signal.h"

#include <glm/glm.hpp>
#include <string> 

//! \class Button
//! \brief A button is a graphic object that can be clicked to trigger an action.
class Button : public GraphicObject
{
    public: 
        Button(GraphicContext* context, const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const std::string& text);
        virtual ~Button();

        virtual void Update() override;
        virtual void Draw() override;

        //! \brief Set the position of the button.
        void SetPosition(const glm::vec2& position);

        //! \brief Set the size of the button.
        void SetSize(const glm::vec2& size);

        //! \brief Set the color of the button.
        void SetColor(const glm::vec3& color);

        //! \brief Set the text of the button.
        void SetText(const std::string& text);

        //! \brief Set the hovering color of the button.
        void SetHoveringColor(const glm::vec3& color);

        glm::vec2 GetPosition() const { return m_Position; }
        glm::vec2 GetSize() const { return m_Size; }
        glm::vec3 GetColor() const { return m_Color; }
        glm::vec3 GetHoveringColor() const { return m_HoveringColor; }
        std::string GetText() const { return m_Text; }

        Signal<> OnClick;
    protected: 
        glm::vec2 m_Position;
        glm::vec2 m_Size;
        glm::vec3 m_Color;
        glm::vec3 m_HoveringColor;
        std::string m_Text;

}