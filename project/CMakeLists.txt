cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:fast")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")

project(ParticleSandbox)

include_directories(
    ${PROJECT_INCLUDE_DIR}
)

# replace \ by / in ${MAIN_DIR} to avoid cmake error
string(REPLACE "\\" "/" MAIN_DIR ${MAIN_DIR})

set(SOURCES

    ../src/arch_support.cpp
    
    ../src/glad/glad.c
    ../src/main.cpp
    
    ../src/common/Application.cpp
    ../src/common/Window.cpp
    ../src/common/EventHandler.cpp
    ../src/common/Scene.cpp
    ../src/common/Object.cpp
    ../src/common/Chrono.cpp

    ../src/graphics/main/GraphicContext.cpp
    ../src/graphics/main/GraphicObject.cpp
    ../src/graphics/main/Shader.cpp
    ../src/graphics/main/Font.cpp
    ../src/graphics/main/ParticleImpl.cpp

    ../src/graphics/util/BasicUI.cpp

    ../src/graphics/components/adapters/TriangleAdapter.cpp
    ../src/graphics/components/adapters/AdvancedRectangle.cpp
    ../src/graphics/components/adapters/RectangleAdapter.cpp
    ../src/graphics/components/adapters/CircleAdapter.cpp
    ../src/graphics/components/adapters/PanickedCircle.cpp
    ../src/graphics/components/adapters/WigglingCircle.cpp
    ../src/graphics/components/common/GraphicText.cpp
    ../src/graphics/components/common/GraphicImage.cpp
    ../src/graphics/components/common/GraphicLine.cpp
    ../src/graphics/components/common/BezierCurve.cpp
    ../src/graphics/components/ui/Slider.cpp
    ../src/graphics/components/ui/Button.cpp
    ../src/graphics/components/ui/Input.cpp
    ../src/graphics/components/ui/Checkbox.cpp
)

add_executable(ParticleSandbox ${SOURCES} favicon.rc)

target_link_libraries(ParticleSandbox
    ${GLFW_DIR}/glfw3.lib
    ${FREETYPE_DIR}/lib/freetyped.lib
)


install(TARGETS ParticleSandbox DESTINATION ${CMAKE_INSTALL_PREFIX})
if(CMAKE_BUILD_TYPE MATCHES Debug)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/ParticleSandbox.pdb DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
# install shaders
install(DIRECTORY ${MAIN_DIR}/src/graphics/shaders DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${MAIN_DIR}/fonts DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${MAIN_DIR}/data DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${MAIN_DIR}/data DESTINATION ${CMAKE_INSTALL_PREFIX})