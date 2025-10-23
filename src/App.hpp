#pragma once

#include "MetalRHI/MetalRHI.hpp"
#include "Window/Window.h"

struct AppParams
{
    std::string name;
    Size2D      windowSize;
};

class App
{
public:
    explicit App(const AppParams& params);

    void run();

    [[nodiscard]] Window*   getWindow() const { return mWindow.get();   }
    [[nodiscard]] MetalRHI* getRHI()    const { return mMetalRHI.get(); }

    [[nodiscard]] static UPtr<App> create(const AppParams& params)
    {
        return std::make_unique<App>(params);
    }

private:
    UPtr<Window>    mWindow;
    UPtr<MetalRHI>  mMetalRHI;
};

extern App* gApp;