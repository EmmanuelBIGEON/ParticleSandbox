#include <iostream>

#include "common/Application.h"
#include "common/Object.h"

#include <emscripten/emscripten.h>
#include <filesystem>
#include <fstream>

#include "arch_support.h"

namespace fs = std::filesystem;

bool g_bIsAVXSupported = false;
bool g_bIsAVX2Supported = false;
bool g_bIsAVX512Supported = false;
bool g_bIsSSSE3Supported = false;
bool g_bIsSSSE4Supported = false;

int main()
{
    EM_ASM(
        Module.canvas = document.getElementById("canvas");
        var aGlCtx = Module.canvas.getContext ('webgl2',  { alpha: true, depth: true, antialias: true, preserveDrawingBuffer: false, stencil: true } );
    );

    // initialize rand
    srand(time(NULL));

    Application* app = Application::GetInstance();
    app->Run();

    emscripten_set_main_loop([](){
        // Not optimal, but it works.
        // Optimization todo
        Application::GetInstance()->Render();
    }, 60, 1);


    return 0;
}

