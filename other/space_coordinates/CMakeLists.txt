cmake_minimum_required(VERSION 3.18)

project(SpaceCoordinates)

include_directories(
    ${PROJECT_INCLUDE_DIR}
)

# replace \ by / in ${MAIN_DIR} to avoid cmake error
string(REPLACE "\\" "/" MAIN_DIR ${MAIN_DIR})


set(SOURCES
    ../../../src/glad/glad.c
    src/main.cpp

    src/common/Window.cpp
    src/common/Shader.cpp
)

add_executable(SpaceCoordinates ${SOURCES})

target_link_libraries(SpaceCoordinates
    ${GLFW_DIR}/glfw3.lib
    ${FREETYPE_DIR}/lib/freetyped.lib
)


install(TARGETS SpaceCoordinates DESTINATION ${CMAKE_INSTALL_PREFIX})
if(CMAKE_BUILD_TYPE MATCHES Debug)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/SpaceCoordinates.pdb DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

install(DIRECTORY src/shaders DESTINATION ${CMAKE_INSTALL_PREFIX})