#pragma once

class GraphicContext;
class Window;
class Wasm_View;
class Scene;

//! \class Application
//! \brief Entry point. Create the window and the graphic context.
//! Load scenes and manage them. (TODO with a Signal/Slot system)
class Application
{
    public:

        void Run();

        //! \brief Render the current scene.
        //! This function is called by the window. (tick = 1/60s)
        void Render(); 

        // Might be useful later to load scenes from a file, for now it's not needed.
        // LoadScene(const std::string& theScenePath);

        void LoadScene_1();
        void LoadScene_2();
        void LoadScene_3();

        static int viewportWidth;
        static int viewportHeight;
        static int fps;

        // singleton
        static Application* GetInstance()
        {
            if(instance == nullptr)
                instance = new Application();
            return instance;
        }

    protected:

        Application();
        virtual ~Application();

        static Application* instance;

#ifdef __EMSCRIPTEN__
        void CreateWasmView();
#else 
        void CreateWindow();
#endif 

    private: 

#ifdef __EMSCRIPTEN__
        Wasm_View* m_WasmView;
#else
        Window* m_Window;
#endif

        GraphicContext* m_GraphicContext;
        Scene* m_currentScene;



};