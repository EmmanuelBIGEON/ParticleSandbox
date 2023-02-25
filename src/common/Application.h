#pragma once

class GraphicContext;
class Window;
class Scene;

//! \class Application
//! \brief Entry point. Create the window and the graphic context.
//! Load scenes and manage them. (TODO with a Signal/Slot system)
class Application
{
    public:
        Application();
        virtual ~Application();

        void Run();

        //! \brief Render the current scene.
        //! This function is called by the window. (tick = 1/60s)
        void Render(); 

        // Might be useful later to load scenes from a file, for now it's not needed.
        // LoadScene(const std::string& theScenePath);

        void LoadScene_1();
        void LoadScene_2();
        void LoadScene_3();


    protected:
        
        virtual void CreateWindow();

    private: 
        Window* m_Window;
        GraphicContext* m_GraphicContext;
        Scene* m_currentScene;



};