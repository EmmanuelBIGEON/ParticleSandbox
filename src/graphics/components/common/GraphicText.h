#pragma once

#include "../../main/Font.h"
#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"
#include "../../main/Shader.h"

#include <string>
#include <glm/glm.hpp>

// Allow to align text in its box.
enum TextAlign
{
    ALIGN_LEFT = 0,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

//! \class GraphicText
//! \brief A class that creates a text object to be displayed.
//! Using Font.cpp (FreeType)
class GraphicText : public GraphicObject
{
    public: 
        GraphicText(GraphicContext* context, const char* text, Font* font, float x, float y, float scale, glm::vec3 color);
        GraphicText(GraphicContext* context, const std::string& text, Font* font, float x, float y, float scale, glm::vec3 color);
        GraphicText(GraphicContext* context, const char* text, Font* font, float xstart, float ystart, float xend, float yend, glm::vec3 color);
        virtual ~GraphicText();

        void SetText(const std::string& text);
        void SetFont(Font* font);

        const std::string& GetText() const { return m_Text; }
        const Font* GetFont() const { return m_Font; }

        void SetColor(glm::vec3 color) { m_Color = color;}

        virtual void Update() override;
        virtual void Draw() override;

    private:
        std::string m_Text;
        Font* m_Font;

        // Needed to update the uniform text color
        Shader* m_Shader;

        float m_x;
        float m_y;

        float m_xstart;
        float m_ystart;
        float m_xend;
        float m_yend;

        float m_scale;

        glm::vec3 m_Color;

        unsigned int m_VAO;
        unsigned int m_VBO;
        // float m_vertices[28];

        // Will contain the vertices of the text
        float* m_vertices;
};