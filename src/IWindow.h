#pragma once

namespace CA
{
    class MetalLayer;
}
namespace MTL
{
    class Device;
}

class IWindow
{
public:
    virtual bool            willClose()                            const = 0;
    virtual CA::MetalLayer* createMetalLayer(MTL::Device* pDevice) const = 0;

    virtual ~IWindow() = default;
};