#pragma once

#include "../../main/GraphicObject.h"
#include "../../main/GraphicContext.h"
#include "../../main/Shader.h"

#include "../../../common/Signal.h"
#include "../common/GraphicText.h"

#include <string>


//! \class Input
//! \brief A class that creates a Input object allowing to retrieve a value
class Input : public GraphicObject
{
    public: 
        Input(GraphicContext* context, std::string placeHolder, float x, float y);
        virtual ~Input();

        void SetValue(std::string pvalue) { m_value = pvalue; m_IsUpdated = false; }
        const std::string& GetValue() const { return m_value; }

        virtual void Update() override;
        virtual void Draw() override;

        void SetNumberOnly(bool numberOnly) { m_numberOnly = numberOnly; }
        void SetMaxSize(int maxSize) { m_maxSize = maxSize; }


        Signal<std::string> OnValueChanged;

    protected:    
        bool isFocused;
        bool isHovered;
        bool m_numberOnly;
        static float height;
        static float width;


        // void OnMouseMoved(float x, float y);
        // void OnMousePressed(float x, float y);
        
    private:

        float m_shiftX;
        float m_shiftY;

        std::string m_value;
        int m_maxSize; // maximum number of characters

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        unsigned int m_VAO_cursor;
        unsigned int m_VBO_cursor;
        unsigned int m_EBO_cursor;

        float m_vertices[8];

        Shader* m_Shader;
        GraphicText* m_ValueTextObject;
        RectangleAdapter* m_FocusRectangle;
};