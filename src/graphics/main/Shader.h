#pragma once

class Shader
{
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        virtual ~Shader();

        void Use();

        // Prevent all graphic objects from always redefining the attributes
        void BindVao();

    protected:
        unsigned int m_ShaderID;
        unsigned int m_Vao;

};