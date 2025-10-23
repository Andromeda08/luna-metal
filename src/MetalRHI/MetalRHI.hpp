#pragma once

#include <array>
#include <memory>
#include <string>

#include <metal/metal.hpp>

#include "IWindow.h"

#pragma region "Shader"
inline auto gFullscreenQuadShader = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut vertex_main(uint vertexID [[vertex_id]]) {
    VertexOut out;

    float2 positions[3] = {
        float2(-1.0, -3.0),
        float2(-1.0,  1.0),
        float2( 3.0,  1.0)
    };

    float2 uvs[3] = {
        float2(0.0, 2.0),
        float2(0.0, 0.0),
        float2(2.0, 0.0)
    };

    out.position = float4(positions[vertexID], 0.0, 1.0);
    out.uv = uvs[vertexID];

    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    return float4(0.243, 0.263, 0.471, 1.0);
})"
;
#pragma endregion

// ============================
// Constants
// ============================
constexpr uint64_t gFramesInFlight = 2;

// ============================
// Type Aliases
// ============================
template <class T>
using UPtr = std::unique_ptr<T>;

template <class T>
using NSPtr = NS::SharedPtr<T>;

template <class T>
using PerFrameArray = std::array<T, gFramesInFlight>;

struct Size2D
{
    uint32_t width  = 0;
    uint32_t height = 0;
};

// ============================
// MetalRHI : Textures
// ============================
#pragma region

struct MetalTextureParams
{
    Size2D              size;
    MTL::PixelFormat    format;
    MTL::Device*        pDevice;
    std::string         debugName;
};

class MetalTexture
{
public:
    explicit MetalTexture(const MetalTextureParams& params);

    static UPtr<MetalTexture> create(const MetalTextureParams& params);

    [[nodiscard]] MTL::Texture* getHandle() const;

    [[nodiscard]] MTL::PixelFormat getFormat() const;

    [[nodiscard]] Size2D getSize() const;

private:
    const Size2D            mSize;
    const MTL::PixelFormat  mFormat;

    MTL::Device*            mDevice;
    NSPtr<MTL::Texture>     mTexture;
};

#pragma endregion

// ============================
// Metal RenderPass Utility
// ============================
struct MTLRenderPass
{
    MTLRenderPass& init();

    MTLRenderPass& addColorAttachment(
        const MTL::Texture*    pTexture,
         uint32_t              index,
         MTL::LoadAction       load        = MTL::LoadActionClear,
         MTL::StoreAction      store       = MTL::StoreActionDontCare,
        const MTL::ClearColor& clearColor  = MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));

    MTLRenderPass& setColorAttachmentTexture(const MTL::Texture* pTexture, uint32_t index);

    NSPtr<MTL4::RenderCommandEncoder> renderCommandEncoder(MTL4::CommandBuffer* pCommandBuffer) const;

    std::vector<NSPtr<MTL::RenderPassColorAttachmentDescriptor>> mColorAttachments;
    NSPtr<MTL4::RenderPassDescriptor>                            mRenderPass;
};

// ============================
// MetalRHI
// ============================
struct MetalRHIParams
{
    IWindow* pWindow = nullptr;
};

class MetalRHI
{
public:
    explicit MetalRHI(const MetalRHIParams& params);

    static UPtr<MetalRHI> create(const MetalRHIParams& params);

    void renderFrame();

private:
    void createPipeline();

    IWindow*                                     mWindow = nullptr;

    NSPtr<MTL::Device>                           mDevice;

    NSPtr<MTL4::CommandQueue>                    mCommandQueue;
    PerFrameArray<NSPtr<MTL4::CommandAllocator>> mCommandAllocators;

    NSPtr<CA::MetalLayer>                        mMetalLayer;
    NSPtr<MTL4::CommandBuffer>                   mCommandBuffer;
    uint64_t                                     mFrameNumber = 0;
    uint64_t                                     mCurrentFrame = 0;
    NSPtr<MTL::SharedEvent>                      mSharedEvent;

    MTLRenderPass                                mRenderPass;
    NSPtr<MTL::Library>                          mLibrary;
    NSPtr<MTL::RenderPipelineState>              mPipelineState;
};
