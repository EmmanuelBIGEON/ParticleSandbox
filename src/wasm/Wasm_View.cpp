#include "Wasm_View.h"

#include <emscripten.h>
#include <iostream>
#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include "../common/Application.h"


//! \brief Récupère la largeur du Canvas
static int jsCanvasWidth()
{
    return EM_ASM_INT({
        return Module.canvas.width;
    });
}

//! \brief Récupère la hauteur du Canvas
static int jsCanvasHeight()
{
    return EM_ASM_INT({
        return Module.canvas.height;
    });
}

static EGLint attribute_list[] =
{
    EGL_RED_SIZE,     8,
    EGL_GREEN_SIZE,   8,
    EGL_BLUE_SIZE,    8,
    EGL_ALPHA_SIZE,   0,
    EGL_DEPTH_SIZE,   24,
    EGL_STENCIL_SIZE, 8,
    EGL_NONE
};

Wasm_View::Wasm_View() : m_windowpixelratio(1)
{
    
    EGLConfig eglconfig = NULL;
    EGLint config_size,major,minor;
    EGLContext contextegl;
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display,&major,&minor);

    if (eglChooseConfig (display, attribute_list, &eglconfig, 1, &config_size) == EGL_TRUE && eglconfig != NULL)
    {
        if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE){
					// std::cout << "GIgl::CreateGLContextEmscripten() eglBindAPI failed" << std::endl;
				}
        EGLint anEglCtxAttribs2[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE, EGL_NONE }; 
        contextegl = eglCreateContext (display, eglconfig, EGL_NO_CONTEXT, anEglCtxAttribs2);  
    }

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, contextegl);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // --------------------- OpenGL settings ---------------------
    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  


    m_pixelratio = emscripten_get_device_pixel_ratio();

    std::string m_CanvasId = "!canvas";
    const EM_BOOL toUseCapture = EM_TRUE;
    EM_ASM(specialHTMLTargets["!canvas"] = Module.canvas);
    const char* aTargetId = "!canvas";
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, toUseCapture, onResizeCallback);
    emscripten_set_mousedown_callback  (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_mouseup_callback    (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_mousemove_callback  (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_dblclick_callback   (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_click_callback      (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_mouseenter_callback (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_mouseleave_callback (aTargetId, this, toUseCapture, onMouseCallback);
    emscripten_set_wheel_callback      (aTargetId, this, toUseCapture, onWheelCallback);
    emscripten_set_touchstart_callback (aTargetId, this, toUseCapture, onTouchCallback);
    emscripten_set_touchend_callback   (aTargetId, this, toUseCapture, onTouchCallback);
    emscripten_set_touchmove_callback  (aTargetId, this, toUseCapture, onTouchCallback);
    emscripten_set_touchcancel_callback(aTargetId, this, toUseCapture, onTouchCallback);
    emscripten_set_keydown_callback    (EMSCRIPTEN_EVENT_TARGET_WINDOW, this, toUseCapture, onKeyDownCallback);
    emscripten_set_keyup_callback      (EMSCRIPTEN_EVENT_TARGET_WINDOW, this, toUseCapture, onKeyUpCallback);

}
Wasm_View::~Wasm_View(){}

void Wasm_View::DumpGLInfo()
{
  // Informations générales OpenGL
  std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  int test = GL_SHADING_LANGUAGE_VERSION;

  // Informations EGL
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  std::cout << "EGL_VERSION: " << eglQueryString((EGLDisplay )display, EGL_VERSION) << std::endl;
  std::cout << "EGL_VENDOR: " << eglQueryString((EGLDisplay )display, EGL_VENDOR) << std::endl;
  std::cout << "EGL_CLIENT_APIS: " << eglQueryString((EGLDisplay )display, EGL_CLIENT_APIS) << std::endl;
}


EM_BOOL Wasm_View::onResizeEvent (int theEventType, const EmscriptenUiEvent* theEvent)
{
    std::cout << "onResizeEvent" << std::endl;
    // get width and height
    int width = 0, height = 0;
    width = jsCanvasWidth();
    height = jsCanvasHeight();
    glViewport(0, 0, width, height);
    Application::viewportWidth = width;
    Application::viewportHeight = height;
  return EM_TRUE;
}


EM_BOOL Wasm_View::onTouchEvent(int theEventType, const EmscriptenTouchEvent* theEvent)
{
  return EM_TRUE;
}

EM_BOOL Wasm_View::onMouseEvent (int theEventType, const EmscriptenMouseEvent* theEvent)
{
  return EM_TRUE;
}

EM_BOOL Wasm_View::onWheelEvent (int theEventType, const EmscriptenWheelEvent* theEvent)
{
  return EM_TRUE;
}

EM_BOOL Wasm_View::onKeyDownEvent (int theEventType, const EmscriptenKeyboardEvent* theEvent)
{
  return EM_TRUE;
}

EM_BOOL Wasm_View::onKeyUpEvent (int theEventType, const EmscriptenKeyboardEvent* theEvent)
{
  return EM_TRUE;
}