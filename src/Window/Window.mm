#include "Window.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include "MetalRHI/MetalRHI.hpp"

CA::MetalLayer* Window::createMetalLayer(MTL::Device* pDevice) const
{
    int width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);

    CA::MetalLayer* pLayer = CA::MetalLayer::layer();
    pLayer->setDevice(pDevice);
    pLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    pLayer->setMaximumDrawableCount(gFramesInFlight);
    pLayer->setDrawableSize(CGSizeMake(static_cast<float>(width), static_cast<float>(height)));

    NSWindow* metalWindow = glfwGetCocoaWindow(mWindow);

    metalWindow.contentView.layer = (__bridge CALayer*) pLayer;
    metalWindow.contentView.wantsLayer = YES;

    return pLayer;
}