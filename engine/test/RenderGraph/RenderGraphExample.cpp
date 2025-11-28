/**
 * @file RenderGraphExample.cpp
 * @brief Example demonstrating RenderGraph Option A API usage
 *
 * This example shows how to use the RenderGraph with Option A (Builder + Compile) style:
 * - Create passes with setup and execute lambdas
 * - Declare resource dependencies
 * - Compile the graph to analyze dependencies
 * - Execute the graph
 */

#include "RenderGraph/RenderGraph.hpp"
#include "RenderGraph/RenderGraphBuilder.hpp"
#include "RenderGraph/RenderGraphResources.hpp"
#include "RHI/RHIDevice.hpp"
#include "RHI/RHIDesc.h"
#include <iostream>

using namespace RenderToy;

/**
 * Example: Simple deferred rendering pipeline
 *
 * Pipeline:
 * 1. Shadow Pass -> creates shadow map
 * 2. GBuffer Pass -> creates albedo, normal, depth
 * 3. Lighting Pass -> reads shadow map and GBuffer, creates final color
 * 4. Present Pass -> copies final color to backbuffer
 */
void renderGraphExample(RHIDevice* device, RHITextureHandle backbuffer) {
    // Create a new render graph for this frame
    RenderGraph graph(device);

    // ========== Shadow Pass ==========
    // Creates a depth texture for shadows
    auto shadowMap = graph.addPass<RGTextureHandle>(
        "ShadowPass",
        // Setup lambda: declare resources and dependencies
        [](RenderGraphBuilder& builder) {
            // Create shadow map texture
            auto shadow = builder.createTexture("ShadowMap", {
                .width = 2048,
                .height = 2048,
                .format = RHITextureFormat::Depth32Float,
                .usage = RHITextureUsageFlags::DepthStencilAttachment,
                .debugName = "ShadowMap"
            });

            // Declare that this pass writes to the shadow map
            builder.writeTexture(shadow);

            // Return the handle so other passes can reference it
            return shadow;
        },
        // Execute lambda: record rendering commands
        [](const RenderGraphResources& resources, RHICommandList& cmd) {
            std::cout << "[ShadowPass] Executing..." << std::endl;

            // Get the actual RHI texture handle
            auto shadowTex = resources.getTexture("ShadowMap");

            // Record rendering commands
            // cmd.beginRenderPass(shadowTex, RHILoadAction::Clear, RHIStoreAction::Store);
            // cmd.setPipelineState(shadowPSO);
            // ... render scene from light's point of view
            // cmd.endRenderPass();
        }
    );

    // ========== GBuffer Pass ==========
    // Creates geometry buffer textures (albedo, normal, depth)
    struct GBufferOutputs {
        RGTextureHandle albedo;
        RGTextureHandle normal;
        RGTextureHandle depth;
    };

    auto gbuffer = graph.addPass<GBufferOutputs>(
        "GBufferPass",
        [](RenderGraphBuilder& builder) {
            GBufferOutputs outputs;

            // Create GBuffer textures
            outputs.albedo = builder.createTexture("GBuffer_Albedo", {
                .width = 1920,
                .height = 1080,
                .format = RHITextureFormat::RGBA8Unorm,
                .usage = RHITextureUsageFlags::RenderTarget | RHITextureUsageFlags::ShaderResource,
                .debugName = "GBuffer_Albedo"
            });

            outputs.normal = builder.createTexture("GBuffer_Normal", {
                .width = 1920,
                .height = 1080,
                .format = RHITextureFormat::RGBA16Float,
                .usage = RHITextureUsageFlags::RenderTarget | RHITextureUsageFlags::ShaderResource,
                .debugName = "GBuffer_Normal"
            });

            outputs.depth = builder.createTexture("GBuffer_Depth", {
                .width = 1920,
                .height = 1080,
                .format = RHITextureFormat::Depth32Float,
                .usage = RHITextureUsageFlags::DepthStencilAttachment,
                .debugName = "GBuffer_Depth"
            });

            // Declare writes
            builder.writeTexture(outputs.albedo);
            builder.writeTexture(outputs.normal);
            builder.writeTexture(outputs.depth);

            return outputs;
        },
        [](const RenderGraphResources& resources, RHICommandList& cmd) {
            std::cout << "[GBufferPass] Executing..." << std::endl;

            // auto albedo = resources.getTexture("GBuffer_Albedo");
            // auto normal = resources.getTexture("GBuffer_Normal");
            // auto depth = resources.getTexture("GBuffer_Depth");

            // cmd.beginRenderPass({albedo, normal}, depth, RHILoadAction::Clear);
            // ... render geometry
            // cmd.endRenderPass();
        }
    );

    // ========== Lighting Pass ==========
    // Reads GBuffer and shadow map, produces final lit image
    auto finalColor = graph.addPass<RGTextureHandle>(
        "LightingPass",
        [&](RenderGraphBuilder& builder) {
            // Declare dependencies on previous passes
            builder.readTexture(shadowMap);       // Read shadow map from ShadowPass
            builder.readTexture(gbuffer.albedo);  // Read GBuffer outputs
            builder.readTexture(gbuffer.normal);
            builder.readTexture(gbuffer.depth);

            // Create output texture
            auto color = builder.createTexture("FinalColor", {
                .width = 1920,
                .height = 1080,
                .format = RHITextureFormat::RGBA16Float,
                .usage = RHITextureUsageFlags::RenderTarget | RHITextureUsageFlags::ShaderResource,
                .debugName = "FinalColor"
            });

            builder.writeTexture(color);
            return color;
        },
        [](const RenderGraphResources& resources, RHICommandList& cmd) {
            std::cout << "[LightingPass] Executing..." << std::endl;

            // auto shadow = resources.getTexture("ShadowMap");
            // auto albedo = resources.getTexture("GBuffer_Albedo");
            // auto normal = resources.getTexture("GBuffer_Normal");
            // auto depth = resources.getTexture("GBuffer_Depth");
            // auto output = resources.getTexture("FinalColor");

            // cmd.beginRenderPass(output, RHILoadAction::DontCare);
            // cmd.setPipelineState(lightingPSO);
            // cmd.setTexture(0, shadow);
            // cmd.setTexture(1, albedo);
            // cmd.setTexture(2, normal);
            // cmd.setTexture(3, depth);
            // cmd.drawFullscreenQuad();
            // cmd.endRenderPass();
        }
    );

    // ========== Present Pass ==========
    // Copies final image to swapchain backbuffer
    graph.addPass<void>(
        "PresentPass",
        [&](RenderGraphBuilder& builder) {
            // Read from lighting pass output
            builder.readTexture(finalColor);

            // Import external backbuffer
            auto bb = builder.importTexture("Backbuffer", backbuffer);
            builder.writeTexture(bb);
        },
        [](const RenderGraphResources& resources, RHICommandList& cmd) {
            std::cout << "[PresentPass] Executing..." << std::endl;

            // auto src = resources.getTexture("FinalColor");
            // auto dst = resources.getTexture("Backbuffer");
            // cmd.copyTexture(src, dst);
        }
    );

    // ========== Compile and Execute ==========
    std::cout << "\n=== Compiling RenderGraph ===" << std::endl;
    graph.compile();

    std::cout << "\n=== Executing RenderGraph ===" << std::endl;
    graph.execute();

    std::cout << "\n=== RenderGraph Complete ===" << std::endl;
}

/**
 * This example demonstrates:
 *
 * 1. **Option A API**: Setup and execute separated into two lambdas
 * 2. **Resource Creation**: createTexture() in setup lambda
 * 3. **Dependency Declaration**: readTexture() / writeTexture()
 * 4. **Resource Sharing**: Previous pass return values used in later passes
 * 5. **External Resources**: importTexture() for swapchain backbuffer
 * 6. **Compile Stage**: Analyzes dependencies, sorts passes, allocates resources
 * 7. **Execute Stage**: Runs passes in dependency order
 *
 * Expected execution order (after compilation):
 * 1. ShadowPass (no dependencies)
 * 2. GBufferPass (no dependencies)
 * 3. LightingPass (depends on ShadowPass and GBufferPass)
 * 4. PresentPass (depends on LightingPass)
 */
