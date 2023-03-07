#include <iostream>

#include "common/Application.h"
#include "common/Object.h"


int main()
{
    // initialize rand
    srand(time(NULL));
    
    // Application* app = new Application();
    Application* app = Application::GetInstance();

    if(app == nullptr)
    {
        std::cout << "Application is null" << std::endl;
        return 1;
    }
    app->Run();
    return 0;
}

