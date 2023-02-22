cmake_minimum_required(VERSION 3.18)

project(ParticleSandbox)

include_directories(
    ${PROJECT_INCLUDE_DIR}
)

# replace \ by / in ${MAIN_DIR} to avoid cmake error
string(REPLACE "\\" "/" MAIN_DIR ${MAIN_DIR})


set(SOURCES
    ../src/glad/glad.c

    ../src/main.cpp
    
    ../src/common/Application.cpp
    ../src/common/Window.cpp
    ../src/common/EventHandler.cpp

    ../src/graphics/main/GraphicContext.cpp
    ../src/graphics/main/GraphicObject.cpp
    ../src/graphics/main/Shader.cpp

    ../src/graphics/adapters/TriangleAdapter.cpp
    ../src/graphics/adapters/RectangleAdapter.cpp
)

add_executable(ParticleSandbox ${SOURCES})

target_link_libraries(ParticleSandbox
    ${GLFW_DIR}/glfw3.lib
    ${FREETYPE_DIR}/
)


install(TARGETS ParticleSandbox DESTINATION ${CMAKE_INSTALL_PREFIX})
if(CMAKE_BUILD_TYPE MATCHES Debug)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/ParticleSandbox.pdb DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
# install shaders
install(DIRECTORY ${MAIN_DIR}/src/graphics/shaders DESTINATION ${CMAKE_INSTALL_PREFIX})