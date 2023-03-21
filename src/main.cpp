#include <iostream>

#include "common/Application.h"
#include "common/Object.h"


#include  "graphics/components/adapters/ParticleAdapter.h"


int main()
{
    // initialize rand
    srand(time(NULL));

    // std::cout << "size of particleAdapter2 : " << sizeof(ParticleAdapter2) << std::endl;
    
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

