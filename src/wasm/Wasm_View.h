#pragma once

#include <string>

#include <emscripten.h>
#include <emscripten/html5.h>

class Wasm_View
{
    public:
        Wasm_View();
        ~Wasm_View();

        void DumpGLInfo();
        void UpdateWindowRatio(double ratio) {m_windowpixelratio = ratio;}

    protected:
        double m_pixelratio;
        double m_windowpixelratio;
        
        EM_BOOL onResizeEvent (int theEventType, const EmscriptenUiEvent* theEvent);
        EM_BOOL onMouseEvent (int theEventType, const EmscriptenMouseEvent* theEvent);
        EM_BOOL onWheelEvent (int theEventType, const EmscriptenWheelEvent* theEvent);
        EM_BOOL onTouchEvent (int theEventType, const EmscriptenTouchEvent* theEvent);
        EM_BOOL onKeyDownEvent (int theEventType, const EmscriptenKeyboardEvent* theEvent);
        EM_BOOL onKeyUpEvent (int theEventType, const EmscriptenKeyboardEvent* theEvent);  

          static EM_BOOL onResizeCallback (int theEventType, const EmscriptenUiEvent* theEvent, void* theView)
        { return ((Wasm_View*)theView)->onResizeEvent (theEventType, theEvent); }

        static EM_BOOL onMouseCallback (int theEventType, const EmscriptenMouseEvent* theEvent, void* theView)
        { return ((Wasm_View* )theView)->onMouseEvent (theEventType, theEvent); }

        static EM_BOOL onWheelCallback (int theEventType, const EmscriptenWheelEvent* theEvent, void* theView)
        { return ((Wasm_View* )theView)->onWheelEvent (theEventType, theEvent); }

        static EM_BOOL onTouchCallback (int theEventType, const EmscriptenTouchEvent* theEvent, void* theView)
        { return ((Wasm_View* )theView)->onTouchEvent (theEventType, theEvent); }

        static EM_BOOL onKeyDownCallback (int theEventType, const EmscriptenKeyboardEvent* theEvent, void* theView)
        { return ((Wasm_View* )theView)->onKeyDownEvent (theEventType, theEvent); }

        static EM_BOOL onKeyUpCallback (int theEventType, const EmscriptenKeyboardEvent* theEvent, void* theView)
        { return ((Wasm_View* )theView)->onKeyUpEvent (theEventType, theEvent); }

};