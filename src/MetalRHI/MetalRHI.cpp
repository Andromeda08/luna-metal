#include "MetalRHI.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <print>

#pragma region "MetalTexture"

MetalTexture::MetalTexture(const MetalTextureParams& params)
: mSize(params.size)
, mFormat(params.format)
, mDevice(params.pDevice)
{
    auto* textureDescriptor = MTL::TextureDescriptor::alloc();
    textureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
    textureDescriptor->setWidth(params.size.width);
    textureDescriptor->setHeight(params.size.height);
    textureDescriptor->setDepth(1);
    textureDescriptor->setTextureType(MTL::TextureType2D);
    textureDescriptor->setMipmapLevelCount(1);
    textureDescriptor->setSampleCount(1u);
    textureDescriptor->setPixelFormat(params.format);
    textureDescriptor->setArrayLength(1); // if 2D must equal 1
    textureDescriptor->setSwizzle(MTL::TextureSwizzleChannels::Default());

    mTexture = NS::TransferPtr(mDevice->newTexture(textureDescriptor));

    const NS::String* label = NS::String::alloc()->init(params.debugName.c_str(), NS::UTF8StringEncoding);
    mTexture->setLabel(label);

    std::println("New MetalTexture[w={}, h={}, l={}]", mSize.width, mSize.height, label->utf8String());
}

UPtr<MetalTexture> MetalTexture::create(const MetalTextureParams& params)
{
    return std::make_unique<MetalTexture>(params);
}

MTL::Texture* MetalTexture::getHandle() const
{
    return mTexture.get();
}

MTL::PixelFormat MetalTexture::getFormat() const
{
    return mFormat;
}

Size2D MetalTexture::getSize() const
{
    return mSize;
}

#pragma endregion

#pragma region "MTLRenderPass"

MTLRenderPass& MTLRenderPass::init()
{
    mRenderPass = NS::TransferPtr(MTL4::RenderPassDescriptor::alloc());
    mRenderPass->init();

    return *this;
}

MTLRenderPass& MTLRenderPass::addColorAttachment(
    const MTL::Texture*    pTexture,
    const uint32_t         index,
    const MTL::LoadAction  load,
    const MTL::StoreAction store,
    const MTL::ClearColor& clearColor)
{
    auto* attachments = mRenderPass->colorAttachments();

    mColorAttachments.push_back(NS::TransferPtr(MTL::RenderPassColorAttachmentDescriptor::alloc()));
    auto* attachment = mColorAttachments.back().get();
    attachment->init();
    attachment->setClearColor(clearColor);
    attachment->setLoadAction(load);
    attachment->setStoreAction(store);
    attachment->setTexture(pTexture);

    attachments->setObject(attachment, index);

    return *this;
}

MTLRenderPass& MTLRenderPass::setColorAttachmentTexture(const MTL::Texture* pTexture, const uint32_t index)
{
    mRenderPass->colorAttachments()->object(index)->setTexture(pTexture);
    return *this;
}

NSPtr<MTL4::RenderCommandEncoder> MTLRenderPass::renderCommandEncoder(MTL4::CommandBuffer* pCommandBuffer) const
{
    return NS::TransferPtr(pCommandBuffer->renderCommandEncoder(mRenderPass.get()));
}

#pragma endregion

#pragma region "MTLPipeline"

UPtr<MTLPipeline> MTLPipeline::createFromBuilder(const MTLPipelineParams& params, const Builder& builder)
{
    auto pipeline = std::make_unique<MTLPipeline>(params);

    pipeline->mShaderEntryPointNames = builder.mShaderEntryPointNames;
    pipeline->mColorAttachments      = builder.mColorAttachments;
    pipeline->mName                  = builder.mName;

    pipeline->createShaderLibrary(builder.mSource.get());
    pipeline->createPipeline();

    return pipeline;
}

void MTLPipeline::createShaderLibrary(const NS::String* pShaderSource)
{
    NS::Error* error = nullptr;

    auto* compileOptions = MTL::CompileOptions::alloc()->init();
    mLibrary = NS::TransferPtr(mDevice->newLibrary(pShaderSource, compileOptions, &error));
    compileOptions->release();

    assert(mLibrary.get());

    for (const auto& [shaderType, entryPoint] : mShaderEntryPointNames)
    {
        auto* pDescriptor = MTL4::LibraryFunctionDescriptor::alloc()->init();
        pDescriptor->setLibrary(mLibrary.get());
        pDescriptor->setName(entryPoint.get());
        mShaderDescriptors[shaderType] = NS::TransferPtr(pDescriptor);
    }
}

void MTLPipeline::createPipeline()
{
    auto* pipelineDescriptor = MTL4::RenderPipelineDescriptor::alloc()->init();
    for (const auto& [shaderType, descriptor] : mShaderDescriptors)
    {
        using enum ShaderType;
        switch (shaderType)
        {
        case Vertex:
            pipelineDescriptor->setVertexFunctionDescriptor(descriptor.get());
            break;
        case Fragment:
            pipelineDescriptor->setFragmentFunctionDescriptor(descriptor.get());
            break;
        default:
            assert(false);
        }
    }
    for (const auto& attachment : mColorAttachments)
    {
        const auto i = static_cast<NS::UInteger>(std::distance(
            std::begin(mColorAttachments),
            std::ranges::find(mColorAttachments, attachment)));
        pipelineDescriptor->colorAttachments()->setObject(attachment.get(), i);
    }

    NS::Error* error = nullptr;
    auto* compilerDescriptor = MTL4::CompilerDescriptor::alloc()->init();
    auto* compiler = mDevice->newCompiler(compilerDescriptor, &error);

    auto* compilerOptions = MTL4::CompilerTaskOptions::alloc()->init();
    mPipelineState = NS::TransferPtr(compiler->newRenderPipelineState(pipelineDescriptor, compilerOptions, &error));

    compilerOptions->release();
    compiler->release();
    compilerDescriptor->release();
}

#pragma endregion

MetalRHI::MetalRHI(const MetalRHIParams& params): mWindow(params.pWindow)
{
    mDevice = NS::TransferPtr(MTL::CreateSystemDefaultDevice());

    if (mWindow)
    {
        CA::MetalLayer* metalLayer = mWindow->createMetalLayer(mDevice.get());
        mMetalLayer = NS::TransferPtr(metalLayer);
    }

    mCommandQueue = NS::TransferPtr(mDevice->newMTL4CommandQueue());
    mCommandBuffer = NS::TransferPtr(mDevice->newCommandBuffer());

    for (auto i = 0; i < gFramesInFlight; i++)
    {
        mCommandAllocators[i] = NS::TransferPtr(mDevice->newCommandAllocator());
    }

    mSharedEvent = NS::TransferPtr(mDevice->newSharedEvent());
    mSharedEvent->setSignaledValue(mCurrentFrame);
    mSharedEvent->setLabel(MTLSTR("MetalFrameSyncEvent"));

    mRenderPass = MTLRenderPass().init();

    mPipeline = MTLPipeline::Builder({ mDevice })
        .addShaderSource(gFullscreenQuadShader)
        .setShaderEntryPoint(ShaderType::Vertex, "vertex_main")
        .setShaderEntryPoint(ShaderType::Fragment, "fragment_main")
        .addColorAttachment(mMetalLayer->pixelFormat())
        .setName("FullscreenQuadPipeline")
        .create();
}

UPtr<MetalRHI> MetalRHI::create(const MetalRHIParams& params)
{
    return std::make_unique<MetalRHI>(params);
}

void MetalRHI::renderFrame()
{
    mFrameNumber += 1;

    if (mFrameNumber >= gFramesInFlight)
    {
        const auto prev = mFrameNumber - gFramesInFlight;
        mSharedEvent->waitUntilSignaledValue(prev, std::numeric_limits<uint64_t>::max());
    }
    mCurrentFrame = (mCurrentFrame + 1) % gFramesInFlight;

    CA::MetalDrawable* metalDrawable = mMetalLayer->nextDrawable();
    mRenderPass.setColorAttachmentTexture(metalDrawable->texture(), 0);

    mCommandAllocators[mCurrentFrame]->reset();
    mCommandBuffer->beginCommandBuffer(mCommandAllocators[mCurrentFrame].get());

    const MTL::Viewport viewport = {
        .originX = 0,
        .originY = 0,
        .width   = static_cast<double>(metalDrawable->texture()->width()),
        .height  = static_cast<double>(metalDrawable->texture()->height()),
        .znear   = 0.0,
        .zfar    = 1.0,
    };

    const NS::SharedPtr<MTL4::RenderCommandEncoder> renderCommandEncoder = mRenderPass.renderCommandEncoder(mCommandBuffer.get());
    {
        renderCommandEncoder->setViewport(viewport);
        mPipeline->bind(renderCommandEncoder.get());
        renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, 0, 3);
    }
    renderCommandEncoder->endEncoding();

    mCommandBuffer->endCommandBuffer();

    const auto* cmd = mCommandBuffer.get();

    mCommandQueue->wait(metalDrawable);
    mCommandQueue->commit(&cmd, 1);
    mCommandQueue->signalDrawable(metalDrawable);

    metalDrawable->present();

    mCommandQueue->signalEvent(mSharedEvent.get(), mFrameNumber);
}
