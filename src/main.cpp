#include <iostream>

#include "common/Application.h"
#include "common/Object.h"

int main()
{
    // initialize rand with timestamp.
    srand(time(NULL));

    // check for support of AVX 512 
    // The program guess that 256 is available. 
    // The application won't work on a computer that doesn't support at least 256.
    // TODO : check for support of 512, and if ok, implement it in the application for better performance.

    Application* app = Application::GetInstance();
    app->Run();

    return 0;
}

