#pragma once

#include "../../main/GraphicObject.h"
#include "../../main/GraphicContext.h"
#include "../../main/Shader.h"

//! \class Slider
//! \brief A class that creates a slider object to be displayed.
// Used to change a value between a min and a max. 
class Slider : public GraphicObject
{
    public: 
        Slider(GraphicContext* context, float min, float max, float x, float y);
        Slider(GraphicContext* context, float min, float max, float value, float x, float y);
        virtual ~Slider();

        void SetValue(float value) { m_Value = value; m_IsUpdated = true; }
        float GetValue() const { return m_Value; }

        void SetMin(float min) { m_Min = min; m_IsUpdated = false; }
        float GetMin() const { return m_Min; }

        void SetMax(float max) { m_Max = max; m_IsUpdated = false; }
        float GetMax() const { return m_Max; }

        virtual void Update() override;
        virtual void Draw() override;

        bool isHovered;
    protected:    
        static float height;
        static float width;

        bool isPressed;

        void OnMouseMoved(float x, float y);
        void OnMousePressed(float x, float y);
        void OnMouseReleased(float x, float y);
        
    private:
        float m_shiftX;
        float m_shiftY;
        float m_shiftCursorX;
        float m_shiftCursorY;

        float m_Min;
        float m_Max;
        float m_Value;

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;


        unsigned int m_VAO_cursor;
        unsigned int m_VBO_cursor;
        unsigned int m_EBO_cursor;

        float m_vertices[8];
        float m_cursorVertices[8];
        float m_boundingBoxCursor[4];

        Shader* m_Shader;
};