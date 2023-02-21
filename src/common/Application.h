#pragma once


class Window;

class Application
{
    public:
        Application();
        virtual ~Application();

        void Run();

    protected:
        
        virtual void CreateWindow();

    private: 
        Window* m_Window;

};