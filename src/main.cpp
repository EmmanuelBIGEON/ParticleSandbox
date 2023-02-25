#include <iostream>

#include "common/Application.h"
#include "common/Object.h"

int main()
{
    // initialize rand
    srand(time(NULL));

    Object* obj = new Object();
    std::cout << obj->GetID().str() << std::endl;

    Object* obj2 = new Object();
    std::cout << obj2->GetID().str() << std::endl;
    
    Application* app = new Application();
    app->Run();
    return 0;
}

