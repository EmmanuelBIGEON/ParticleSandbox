cmake_minimum_required(VERSION 3.18)

project(ParticleSandboxWasm)
set(CMAKE_CXX_STANDARD 17)

# Named inc because include is a function in cmake.. a bit ugly. TODO : find a better way to do this.
include_directories(
    inc
)

# Basic flags for the compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1") # Using WASM
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s MAX_WEBGL_VERSION=2") # Using WebGL 2.0 (OpenGL ES 3.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w") # Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msimd128") # SIMD instructions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")

# Set targetted optimization level
# 0 = no optimization
# 1 = -O1
# 2 = -O2
# 3 = -O3 (highest optimization level)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3") 

# Load necessary files into the WASM data. (shaders and images) 
# ../../src/graphics/shaders@/shaders meaning the shaders folder is binded to the /shaders folder in the WASM Virtual File System)
# Linking paths calculated from the build directory
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ../../../data/@/data")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ../../../fonts/@/fonts")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ../../../src/wasm/shaders/@/shaders") 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s MODULARIZE=1") # using a function to create the module named "createParticleSandbox" (file : wasmloader.js )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORT_NAME='createParticleSandbox'")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --extern-post-js ../wasmloader.js")

# Multithreading : disabled for simple examples
# https://emscripten.org/docs/porting/pthreads.html
# - Need  Cross Origin Opener Policy (COOP) and Cross Origin Embedder Policy (COEP) -
# - To make it work without too much work on setting up the HTTP Server, it is disabled. -
# ---
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS") # Multithread
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s PTHREAD_POOL_SIZE=4") # Max number of threads
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_PTHREADS") # using threads
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s PTHREAD_POOL_SIZE=4")


set(SOURCES
    ../../src/wasm_main.cpp
    ../../src/arch_support.cpp

    ../../src/wasm/Wasm_View.cpp
    
    ../../src/common/Application.cpp
    # ../../src/common/Window.cpp # Replaced by Wasm_View
    ../../src/common/EventHandler.cpp
    ../../src/common/Scene.cpp
    ../../src/common/Object.cpp
    ../../src/common/Chrono.cpp

    ../../src/graphics/main/GraphicContext.cpp
    ../../src/graphics/main/GraphicObject.cpp
    ../../src/graphics/main/Shader.cpp
    ../../src/graphics/main/Font.cpp
    ../../src/graphics/main/ParticleImpl.cpp

    ../../src/graphics/components/ui/layout/BasicUI.cpp
    ../../src/graphics/components/ui/layout/BehaviorUI.cpp

    ../../src/graphics/components/adapters/TriangleAdapter.cpp
    ../../src/graphics/components/adapters/AdvancedRectangle.cpp
    ../../src/graphics/components/adapters/RectangleAdapter.cpp
    ../../src/graphics/components/adapters/CircleAdapter.cpp
    ../../src/graphics/components/adapters/PanickedCircle.cpp
    ../../src/graphics/components/adapters/WigglingCircle.cpp
    ../../src/graphics/components/common/GraphicText.cpp
    ../../src/graphics/components/common/GraphicImage.cpp
    ../../src/graphics/components/common/GraphicLine.cpp
    ../../src/graphics/components/common/BezierCurve.cpp
    ../../src/graphics/components/ui/Button.cpp
    ../../src/graphics/components/ui/Input.cpp
    ../../src/graphics/components/ui/Slider.cpp
    ../../src/graphics/components/ui/Checkbox.cpp
)

add_executable(ParticleSandboxWasm ${SOURCES})

# Link necessary libraries (static librairies only since we are using WASM)
target_link_libraries(ParticleSandboxWasm PRIVATE ${LIB_DIR}/freetype/2.10.0/libfreetype.a)

install(TARGETS ParticleSandboxWasm DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ParticleSandboxWasm.wasm DESTINATION ${CMAKE_INSTALL_PREFIX}) 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ParticleSandboxWasm.worker.js DESTINATION ${CMAKE_INSTALL_PREFIX}) 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ParticleSandboxWasm.data DESTINATION ${CMAKE_INSTALL_PREFIX})

# from source dir
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ParticleSandbox.html DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/wasmloader.js DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico DESTINATION ${CMAKE_INSTALL_PREFIX})
