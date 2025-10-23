#include "MetalRHI/MetalRHI.hpp"
#include "Window/Window.h"

int main()
{
    const auto window = std::make_unique<Window>(1280, 720);
    const auto metalRHI = MetalRHI::create({
        .pWindow = window.get(),
    });

    while (!window->willClose())
    {
        glfwPollEvents();
        metalRHI->renderFrame();
    }

    return 0;
}
