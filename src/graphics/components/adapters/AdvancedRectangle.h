#pragma once

#include "../../main/GraphicContext.h"
#include "../../main/GraphicObject.h"

#include "../../../geometry/Rectangle.h"

#include <glm/glm.hpp>

//! \class AdvancedRectangle
//! \brief A class that creates a rectangle object that accept lighting.
class AdvancedRectangle : public GraphicObject
{
    public: 
        AdvancedRectangle(GraphicContext* context, const Rectangle& rectangle, const glm::vec3& color);
        virtual ~AdvancedRectangle();

        const Rectangle& GetRectangle() const { return m_Rectangle; }
        void SetRectangle(const Rectangle& rectangle) { m_Rectangle = rectangle; m_IsUpdated=false;}
        void SetColor(const glm::vec3& color) { m_Color = color; m_IsUpdated=false;}

        virtual void Update() override;
        virtual void Draw() override;
    
    private:
            
            Rectangle m_Rectangle;
            glm::vec3 m_Color;
    
            float m_vertices[35];
    
            unsigned int m_VAO;
            unsigned int m_VBO; 
};