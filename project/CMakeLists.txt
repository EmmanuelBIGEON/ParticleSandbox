cmake_minimum_required(VERSION 3.18)

project(ParticleSandbox)

include_directories(
    ${PROJECT_INCLUDE_DIR}
)

set(SOURCES
    ../src/glad/glad.c

    ../src/main.cpp
    
    ../src/common/Application.cpp
    ../src/common/Window.cpp
)

add_executable(ParticleSandbox ${SOURCES})

target_link_libraries(ParticleSandbox
    ${GLFW_DIR}/glfw3.lib
)


install(TARGETS ParticleSandbox DESTINATION ${CMAKE_INSTALL_PREFIX})