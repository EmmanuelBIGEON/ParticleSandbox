#pragma once

#include "GraphicContext.h"

#include "../../common/Object.h"

//! \class GraphicObject
//! \brief Base class for all graphic objects.
//! Don't forget to unbind it from the context when you delete it.
class GraphicObject : public Object
{
    public:
        GraphicObject(GraphicContext* context, AvailableShader shaderIndex);
        virtual ~GraphicObject();

        //! \brief Compute graphical representation of the object.
        //! Needed to be called before Draw().
        virtual void Update();

        //! \brief Draw the object.
        virtual void Draw();

        virtual void SetToBeDisplayed(bool toBeDisplayed);
        bool IsDisplayable() const { return m_IsToBeDisplayed;}

        //! Give the identifier of the shader used by the object.
        int GetShaderIndex() const;

        bool IsUpdated() const { return m_IsUpdated; }
        void SetNeedUpdate() { m_IsUpdated = false; }
        
    protected:
        GraphicContext* m_Context;
        int m_ShaderIndex;

        bool m_IsUpdated;
        // bool m_IsDrawn; // maybe later
        bool m_IsToBeDisplayed;
};