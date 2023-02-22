#include "GraphicObject.h"


GraphicObject::GraphicObject(GraphicContext* context, AvailableShader shaderIndex) : m_Context(context), m_ShaderIndex(shaderIndex), m_IsUpdated(false)
{
    context->Register(this);
}

GraphicObject::~GraphicObject()
{
}

void GraphicObject::Update()
{
    // Do nothing, to be implemented by derived classes.
}

void GraphicObject::Draw()
{
    // Do nothing, to be implemented by derived classes.
}

int GraphicObject::GetShaderIndex() const
{
    return m_ShaderIndex;
}
