#include "App.hpp"

App* gApp = nullptr;

App::App(const AppParams& params)
{
    mWindow = std::make_unique<Window>(params.windowSize.width, params.windowSize.height, params.name);
    mMetalRHI = MetalRHI::create({
        .pWindow = mWindow.get(),
    });

    gApp = this;
}

void App::run()
{
    while (!mWindow->willClose())
    {
        glfwPollEvents();
        mMetalRHI->renderFrame();
    }
}
