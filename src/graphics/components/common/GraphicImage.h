#pragma once

#include "../../main/GraphicObject.h"
#include "../../main/GraphicContext.h"

#include <string>


class Shader;

//! \class GraphicImage
//! \brief A class that creates an image object to be displayed.
class GraphicImage : public GraphicObject
{
    public:
        GraphicImage(GraphicContext* context, const std::string& path, float x, float y, float width, float height);
        virtual ~GraphicImage();

        virtual void Update() override;
        virtual void Draw() override;

        void SetPosition(float x, float y);
        void SetWidth(float width);
        void SetHeight(float height);
        void SetImage(const std::string& path);
        void SetToInvertColor(bool invert);

    private:
        Shader* m_Shader;

        std::string m_Path;

        float m_x;
        float m_y;
        float m_width;
        float m_height;
        bool m_imageloaded;

        float m_vertices[32];
        int m_indices[6];

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;
        unsigned int m_Texture;

        bool m_ToInvert;
        
};