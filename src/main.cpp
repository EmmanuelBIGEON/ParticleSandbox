#include <iostream>

#include "common/Application.h"
#include "common/Object.h"


int main()
{
    // initialize rand
    srand(time(NULL));
    
    Application* app = new Application();
    app->Run();
    return 0;
}

