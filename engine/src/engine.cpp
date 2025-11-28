#include <print>
#include "Engine.hpp"
#include "IGame.hpp"
#include "ECS/AnimationSystem.hpp"
#include "ECS/PhysicsSystem.hpp"
#include "ECS/RenderSystem.hpp"
#include "ECS/TransformSystem.hpp"
#include "ECS/UISystem.hpp"

#include "Log/Log.hpp"
#include "Log/ConsoleSink.hpp"
#include "Log/SDLSink.hpp"

#if RHI_METAL
#include "RHI/Metal/MetalDevice_CPPWrapper.hpp"
#include "RHI/Metal/MetalCommandList_CPPWrapper.hpp"

// Metal shader for textured quad rendering
static const char* texturedQuadShaderSource = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float2 position [[attribute(0)]];
    float2 texCoord [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};

vertex VertexOut vertex_main(VertexIn in [[stage_in]]) {
    VertexOut out;
    out.position = float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]],
                             texture2d<float> colorTexture [[texture(0)]]) {
    constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
    float4 color = colorTexture.sample(textureSampler, in.texCoord);
    return color;
}
)";
#endif

namespace RenderToy
{
    int Engine::run(IGame& game){
        Logger::instance().addSink(std::make_unique<ConsoleSink>());
        LOG_DEBUG(LOG_CORE, "Engine Start");

        EngineConfig config;
        game.onConfigure(config);

        WindowDesc windowDesc{
            .title = config.windowTitle,
            .width = config.width,
            .height = config.height,
            .resizable = config.resizable
        };
        if(!window.init(windowDesc))
            return -1;

        // Initialize RHI
        LOG_INFO(LOG_CORE, "Initializing RHI...");
        RHIDeviceCreateDesc rhiDesc{
            .enableDebugLayer = false,
            .enableGPUValidation = false,
            .enableValidation = false,
            .applicationName = "RenderToy",
            .windowHandle = nullptr
        };
        rhiDesc.enableValidation = true;  // Enable validation in debug builds
        rhiDesc.windowHandle = window.nativeHandle().metalLayer;

        rhiDevice = RHIDevice::create(rhiDesc);
        if (!rhiDevice) {
            LOG_ERROR(LOG_CORE, "Failed to create RHI device");
            return -1;
        }

        // Create swapchain
        swapchain = std::make_unique<RHISwapchainWrapper>(
            rhiDevice.get(),
            window.nativeHandle().metalLayer,
            window.getWidth(),
            window.getHeight(),
            RHITextureFormat::BGRA8_UNORM,
            3,  // Triple buffering
            true  // VSync enabled
        );

        if (!swapchain->isValid()) {
            LOG_ERROR(LOG_CORE, "Failed to create swapchain");
            return -1;
        }

        // Create RHI helper systems
        framePacer = std::make_unique<RHIFramePacer>(rhiDevice.get());
        commandPool = std::make_unique<RHICommandListPool>(rhiDevice.get());
        pipelineCache = std::make_unique<RHIPipelineCache>(rhiDevice.get());

        LOG_INFO(LOG_CORE, "RHI initialized successfully");

#if RHI_METAL
        // Create textured quad rendering resources
        RHIBufferHandle quadVertexBuffer;
        RHITextureHandle checkerboardTexture;
        RHIShaderHandle vertexShader;
        RHIShaderHandle fragmentShader;
        RHIPipelineStateHandle quadPipeline;

        {
            // Quad vertex data: position (xy) + texCoord (uv)
            struct Vertex {
                float position[2];
                float texCoord[2];
            };

            // Two triangles forming a quad (covering the screen)
            Vertex quadVertices[] = {
                // First triangle
                {{ -0.8f,  0.8f }, { 0.0f, 0.0f }},  // Top left
                {{ -0.8f, -0.8f }, { 0.0f, 1.0f }},  // Bottom left
                {{  0.8f, -0.8f }, { 1.0f, 1.0f }},  // Bottom right

                // Second triangle
                {{ -0.8f,  0.8f }, { 0.0f, 0.0f }},  // Top left
                {{  0.8f, -0.8f }, { 1.0f, 1.0f }},  // Bottom right
                {{  0.8f,  0.8f }, { 1.0f, 0.0f }},  // Top right
            };

            // Create vertex buffer
            RHIBufferCreateDesc bufferDesc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            bufferDesc.size = sizeof(quadVertices);
            bufferDesc.usage = RHIBufferUsageFlags::BufVertexBuffer;
            bufferDesc.initialData = quadVertices;
            bufferDesc.debugName = "Quad Vertex Buffer";

            quadVertexBuffer = rhiDevice->createBuffer(bufferDesc);
            LOG_INFO(LOG_CORE, "Created quad vertex buffer");

            // Create checkerboard texture
            const uint32_t texWidth = 256;
            const uint32_t texHeight = 256;
            const uint32_t checkSize = 32;  // Size of each checker square

            std::vector<uint8_t> textureData(texWidth * texHeight * 4);  // RGBA8
            for (uint32_t y = 0; y < texHeight; ++y) {
                for (uint32_t x = 0; x < texWidth; ++x) {
                    uint32_t index = (y * texWidth + x) * 4;

                    // Checkerboard pattern
                    bool isWhite = ((x / checkSize) + (y / checkSize)) % 2 == 0;
                    uint8_t color = isWhite ? 255 : 64;

                    textureData[index + 0] = color;      // R
                    textureData[index + 1] = color;      // G
                    textureData[index + 2] = color;      // B
                    textureData[index + 3] = 255;        // A
                }
            }

            RHITextureCreateDesc texDesc{
                .width = 0,
                .height = 0,
                .depth = 1,
                .mipLevels = 1,
                .arraySize = 1,
                .format = RHITextureFormat::Unknown,
                .usage = RHITextureUsageFlags::TexNone,
                .initialState = RHIResourceState::Common,
                .clearColor = {
                    .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
                },
                .clearDepthStencil = {
                    .depth = 1.0f,
                    .stencil = 0
                },
                .debugName = nullptr
            };
            texDesc.width = texWidth;
            texDesc.height = texHeight;
            texDesc.depth = 1;
            texDesc.mipLevels = 1;
            texDesc.arraySize = 1;
            texDesc.format = RHITextureFormat::RGBA8_UNORM;
            texDesc.usage = RHITextureUsageFlags::TexShaderResource;
            texDesc.debugName = "Checkerboard Texture";

            checkerboardTexture = rhiDevice->createTexture(texDesc);
            rhiDevice->uploadTextureData(checkerboardTexture, textureData.data(),
                                        textureData.size(), 0, 0);
            LOG_INFO(LOG_CORE, "Created checkerboard texture");

            // Create vertex shader
            RHIShaderCreateDesc vsDesc{
                .stage = RHIShaderStage_Vertex,
                .bytecode = texturedQuadShaderSource,
                .bytecodeSize = strlen(texturedQuadShaderSource),
                .entryPoint = "vertex_main",
                .debugName = "Textured Quad Vertex Shader"
            };

            vertexShader = rhiDevice->createShader(vsDesc);
            LOG_INFO(LOG_CORE, "Created vertex shader");

            // Create fragment shader
            RHIShaderCreateDesc fsDesc{
                .stage = RHIShaderStage_Pixel,
                .bytecode = texturedQuadShaderSource,
                .bytecodeSize = strlen(texturedQuadShaderSource),
                .entryPoint = "fragment_main",
                .debugName = "Textured Quad Fragment Shader"
            };

            fragmentShader = rhiDevice->createShader(fsDesc);
            LOG_INFO(LOG_CORE, "Created fragment shader");

            // Create vertex layout
            RHIVertexElement vertexElements[] = {
                {
                    .semanticName = "POSITION",
                    .semanticIndex = 0,
                    .format = RHITextureFormat::RG32_FLOAT,  // float2 position
                    .inputSlot = 0,
                    .alignedByteOffset = 0,
                    .classification = RHIInputClassification::PerVertex,
                    .instanceDataStepRate = 0
                },
                {
                    .semanticName = "TEXCOORD",
                    .semanticIndex = 0,
                    .format = RHITextureFormat::RG32_FLOAT,  // float2 texCoord
                    .inputSlot = 0,
                    .alignedByteOffset = 8,  // offset after position (2 * sizeof(float))
                    .classification = RHIInputClassification::PerVertex,
                    .instanceDataStepRate = 0
                }
            };

            RHIVertexLayout vertexLayout{
                .elements = vertexElements,
                .elementCount = 2
            };
            vertexLayout.elements = vertexElements;
            vertexLayout.elementCount = 2;

            // Create pipeline state
            RHIGraphicsPipelineStateDesc psoDesc{};
            psoDesc.vertexShaderIndex = vertexShader.index;
            psoDesc.pixelShaderIndex = fragmentShader.index;
            psoDesc.vertexLayout = vertexLayout;
            psoDesc.topology = RHIPrimitiveTopology::TriangleList;
            psoDesc.renderTargetFormats[0] = RHITextureFormat::BGRA8_UNORM;
            psoDesc.renderTargetCount = 1;
            psoDesc.debugName = "Textured Quad Pipeline";

            quadPipeline = rhiDevice->createGraphicsPipelineState(psoDesc);
            LOG_INFO(LOG_CORE, "Created textured quad pipeline state");
        }
#endif

        world.addSystem<AnimationSystem>();
        world.addSystem<PhysicsSystem>();
        world.addSystem<RenderSystem>();
        world.addSystem<TransformSystem>();
        world.addSystem<UISystem>();

        game.onInit(world);

        timer.reset();

        bool isRunning = true;
        while(isRunning && !window.getShouldClose()){
            window.pumpEvents([&](const WindowEvent& event){
                if(event.type == WindowEvent::Type::Quit)
                    isRunning = false;
                else if(event.type == WindowEvent::Type::Resized){
                    // Handle window resize
                    swapchain->resize(event.width, event.height);
                }
            });

            timer.newFrame();

            UpdateContext updateContext{
                .deltaTime = timer.deltaSeconds(),
            };
            game.onUpdate(updateContext);

            world.update(updateContext.deltaTime);

            // Begin frame
            if (framePacer->beginFrame()) {
                // Acquire next drawable first (before recording commands)
                swapchain->present();  // This acquires the next drawable

                // Record and submit rendering commands
                RHICommandList* cmdList = rhiDevice->beginCommandList();
                if (cmdList) {
#if RHI_METAL
                    // Metal-specific: Set the drawable on the command list
                    auto* metalDevice = static_cast<MetalDevice*>(rhiDevice.get());
                    auto* metalCmdList = static_cast<MetalCommandList*>(cmdList);

                    // Get the current drawable from the swapchain
                    auto drawable = metalDevice->getSwapchainDrawable(swapchain->getHandle());
                    if (drawable) {
                        metalCmdList->setDrawable(drawable);

                        // Clear screen to a nice blue color
                        RHIClearColor clearColor{ 0.1f, 0.2f, 0.4f, 1.0f };  // Dark blue

                        // Begin render pass with clear
                        cmdList->beginRenderPass(
                            RHITextureHandle{},  // Using drawable set above
                            RHITextureHandle{},  // No depth stencil yet
                            RHILoadAction_Clear,
                            RHIStoreAction_Store,
                            clearColor
                        );

                        // Draw textured quad
                        cmdList->setPipelineState(quadPipeline);
                        cmdList->setVertexBuffer(0, quadVertexBuffer, sizeof(float) * 4, 0);
                        cmdList->setTexture(0, checkerboardTexture, RHIShaderStage_Pixel);
                        cmdList->draw(6, 1, 0, 0);  // 6 vertices (2 triangles), 1 instance

                        // End render pass
                        cmdList->endRenderPass();
                    }
#endif
                    // Submit command list (this will present the drawable in Metal)
                    rhiDevice->submitCommandList(cmdList);
                }

                framePacer->endFrame();
            }
        }

        // Wait for GPU to finish before cleanup
        LOG_INFO(LOG_CORE, "Waiting for GPU to finish...");
        framePacer->waitForIdle();
        rhiDevice->waitForIdle();

#if RHI_METAL
        // Clean up textured quad resources
        rhiDevice->destroyPipelineState(quadPipeline);
        rhiDevice->destroyShader(fragmentShader);
        rhiDevice->destroyShader(vertexShader);
        rhiDevice->destroyTexture(checkerboardTexture);
        rhiDevice->destroyBuffer(quadVertexBuffer);
        LOG_INFO(LOG_CORE, "Cleaned up textured quad resources");
#endif

        game.onShutdown();

        return 0;
    }
}