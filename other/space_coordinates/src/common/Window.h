#pragma once

class Shader;

class Window
{
    public:
        Window();
        virtual ~Window();

        void Display();

    private:
        Shader* m_Shader_test1;
};
