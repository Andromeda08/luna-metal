#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "IWindow.h"

class Window final : public IWindow
{
public:
    Window(uint32_t width, uint32_t height);

    ~Window() override;

    bool willClose() const override;;

    /**
     * Get the underlying NSWindow from GLFW and use it to create a CAMetalLayer.
     */
    CA::MetalLayer* createMetalLayer(MTL::Device* pDevice) const override;

private:
    static void defaultKeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* mWindow = nullptr;
};
