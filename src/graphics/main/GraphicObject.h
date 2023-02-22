#pragma once

#include "GraphicContext.h"

//! \class GraphicObject
//! \brief Base class for all graphic objects.
class GraphicObject
{
    public:
        GraphicObject(GraphicContext* context, AvailableShader shaderIndex);
        virtual ~GraphicObject();

        //! \brief Compute graphical representation of the object.
        //! Needed to be called before Draw().
        virtual void Update();

        //! \brief Draw the object.
        virtual void Draw();

        //! Give the identifier of the shader used by the object.
        int GetShaderIndex() const;

        bool IsUpdated() const { return m_IsUpdated; }
        void SetNeedUpdate() { m_IsUpdated = false; }
        
    protected:
        GraphicContext* m_Context;
        int m_ShaderIndex;

        bool m_IsUpdated;
        // bool m_IsDrawn; // maybe later
};