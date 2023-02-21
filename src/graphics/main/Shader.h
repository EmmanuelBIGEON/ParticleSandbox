#pragma once

class Shader
{
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        virtual ~Shader();

        virtual void Use();

    protected:
        unsigned int m_ShaderID;

};