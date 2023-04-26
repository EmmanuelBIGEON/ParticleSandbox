#pragma once

class GraphicContext;
class Window;
class Wasm_View;
class Scene;

//! \class Application
//! \brief Entry point. Create the window and the graphic context.
class Application
{
    public:

        void Run();

        //! \brief Render the current scene.
        //! This function is called by the window loop.
        void Render(); 
        
        
        // see Scene.h for details about scene and their purpose.

        void LoadScene_Main();
        void LoadScene_Behavior();
        void LoadScene_Text();
        void LoadScene_Testing();
        void LoadScene_1();
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