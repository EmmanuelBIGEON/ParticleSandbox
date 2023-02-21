#include "GraphicObject.h"


GraphicObject::GraphicObject(GraphicContext* context, int shaderIndex)
{
}

GraphicObject::~GraphicObject()
{
}

void GraphicObject::Draw()
{
    // Do nothing, to be implemented by derived classes.
}

int GraphicObject::GetShaderIndex() const
{
    return m_ShaderIndex;
}
