#pragma once

class GraphicContext;

//! \class GraphicObject
//! \brief Base class for all graphic objects.
class GraphicObject
{
    public:
        GraphicObject(GraphicContext* context, int shaderIndex);
        virtual ~GraphicObject();

        //! \brief Draw the object.
        virtual void Draw();

        //! Give the identifier of the shader used by the object.
        int GetShaderIndex() const;

    protected:
        GraphicContext* m_Context;
        int m_ShaderIndex;
};