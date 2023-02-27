#include <iostream>

#include "common/Window.h"
int main()
{
    std::cout << "Hello World! space_coordinates" << std::endl;

    Window* window = new Window();
    window->Display();
    return 0;
}