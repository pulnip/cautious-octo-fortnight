import Metal
import QuartzCore
import Foundation

// MARK: - Metal CommandList Class

class MetalCommandList {
    var commandBuffer: MTLCommandBuffer?
    var renderEncoder: MTLRenderCommandEncoder?
    var computeEncoder: MTLComputeCommandEncoder?
    var blitEncoder: MTLBlitCommandEncoder?
    var currentDrawable: CAMetalDrawable?
    var commandQueue: MTLCommandQueue?
    var device: UnsafeMutableRawPointer? // MetalDevice pointer
    var isRecording: Bool = false

    func cleanup() {
        if renderEncoder != nil {
            renderEncoder?.endEncoding()
            renderEncoder = nil
        }
        if computeEncoder != nil {
            computeEncoder?.endEncoding()
            computeEncoder = nil
        }
        if blitEncoder != nil {
            blitEncoder?.endEncoding()
            blitEncoder = nil
        }
        commandBuffer = nil
        currentDrawable = nil
    }

    deinit {
        cleanup()
    }
}

// MARK: - C Bridge Functions

@_cdecl("MetalCommandList_create")
func MetalCommandList_create(
    _ commandQueue: UnsafeMutableRawPointer,
    _ devicePtr: MetalDevicePtr
) -> MetalCommandListPtr? {
    autoreleasepool{
        let cmdList = MetalCommandList()

        // Unwrap MTLCommandQueue from opaque pointer
        let queue = Unmanaged<MTLCommandQueue>
            .fromOpaque(commandQueue).takeUnretainedValue()
        cmdList.commandQueue = queue
        cmdList.device = devicePtr

        return Unmanaged.passRetained(cmdList).toOpaque()
    }
}

@_cdecl("MetalCommandList_destroy")
func MetalCommandList_destroy(
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeRetainedValue()
        cmdList.cleanup()
    }
}

@_cdecl("MetalCommandList_begin")
func MetalCommandList_begin(
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        if cmdList.isRecording {
            print("[Metal] Command list already recording")
            return
        }

        guard let queue = cmdList.commandQueue else {
            print("[Metal] No command queue available")
            return
        }

        cmdList.commandBuffer = queue.makeCommandBuffer()
        cmdList.commandBuffer?.label = "RenderToy Command Buffer"
        cmdList.isRecording = true
    }
}

@_cdecl("MetalCommandList_close")
func MetalCommandList_close(
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        if !cmdList.isRecording {
            print("[Metal] Command list not recording")
            return
        }

        // End any active encoder
        if let encoder = cmdList.renderEncoder {
            encoder.endEncoding()
            cmdList.renderEncoder = nil
        }
        if let encoder = cmdList.computeEncoder {
            encoder.endEncoding()
            cmdList.computeEncoder = nil
        }
        if let encoder = cmdList.blitEncoder {
            encoder.endEncoding()
            cmdList.blitEncoder = nil
        }

        // Present drawable if we have one
        if let drawable = cmdList.currentDrawable {
            cmdList.commandBuffer?.present(drawable)
            cmdList.currentDrawable = nil
        }

        // Commit command buffer
        cmdList.commandBuffer?.commit()
        cmdList.isRecording = false
    }
}

@_cdecl("MetalCommandList_reset")
func MetalCommandList_reset(
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()
        cmdList.cleanup()
        cmdList.isRecording = false
    }
}

// MARK: - Render Pass Functions

@_cdecl("MetalCommandList_beginRenderPass")
func MetalCommandList_beginRenderPass(
    _ cmdListPtr: MetalCommandListPtr,
    _ renderTarget: UInt64,
    _ depthStencil: UInt64,
    _ loadAction: RHILoadAction,
    _ storeAction: RHIStoreAction,
    _ clearColorPtr: UnsafePointer<RHIClearColor>?
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard cmdList.isRecording else {
            print("[Metal] Cannot begin render pass - not recording")
            return
        }

        guard cmdList.renderEncoder == nil else {
            print("[Metal] Render pass already active")
            return
        }

        guard let drawable = cmdList.currentDrawable else {
            print("[Metal] No drawable set for render pass")
            return
        }

        let passDescriptor = MTLRenderPassDescriptor()

        // Configure color attachment
        passDescriptor.colorAttachments[0].texture = drawable.texture

        // Set load action
        switch loadAction {
        case RHILoadAction_Load:
            passDescriptor.colorAttachments[0].loadAction = .load
        case RHILoadAction_Clear:
            passDescriptor.colorAttachments[0].loadAction = .clear
            if let clearColor = clearColorPtr?.pointee {
                passDescriptor.colorAttachments[0].clearColor = MTLClearColor(
                    red: Double(clearColor.r),
                    green: Double(clearColor.g),
                    blue: Double(clearColor.b),
                    alpha: Double(clearColor.a))
            }
        default: // DontCare
            passDescriptor.colorAttachments[0].loadAction = .dontCare
        }

        // Set store action
        switch storeAction {
        case RHIStoreAction_Store:
            passDescriptor.colorAttachments[0].storeAction = .store
        default: // DontCare
            passDescriptor.colorAttachments[0].storeAction = .dontCare
        }

        // Create render encoder
        if let cmdBuffer = cmdList.commandBuffer {
            cmdList.renderEncoder = cmdBuffer.makeRenderCommandEncoder(descriptor: passDescriptor)
            cmdList.renderEncoder?.label = "RenderToy Render Pass"
            print("[Metal] Render pass started with drawable texture")
        } else {
            print("[Metal] ERROR: No command buffer for render encoder")
        }
    }
}

@_cdecl("MetalCommandList_beginRenderPassMultiTarget")
func MetalCommandList_beginRenderPassMultiTarget(
    _ cmdListPtr: MetalCommandListPtr,
    _ renderTargets: UnsafePointer<UInt64>,
    _ renderTargetCount: UInt32,
    _ depthStencil: UInt64,
    _ loadAction: RHILoadAction,
    _ storeAction: RHIStoreAction,
    _ clearColorsPtr: UnsafePointer<RHIClearColor>?
) {
    autoreleasepool{
        // For now, just use the first render target
        if renderTargetCount > 0 {
            let clearColor = clearColorsPtr != nil ? clearColorsPtr : nil
            MetalCommandList_beginRenderPass(
                cmdListPtr,
                renderTargets[0],
                depthStencil,
                loadAction,
                storeAction,
                clearColor
            )
        }
    }
}

@_cdecl("MetalCommandList_endRenderPass")
func MetalCommandList_endRenderPass(
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else {
            print("[Metal] No active render pass to end")
            return
        }

        encoder.endEncoding()
        cmdList.renderEncoder = nil
    }
}

// MARK: - Pipeline State Functions

@_cdecl("MetalCommandList_setPipelineState")
func MetalCommandList_setPipelineState(
    _ cmdListPtr: MetalCommandListPtr,
    _ pso: MetalPipelineStateID
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else {
            print("[Metal] No active render encoder for setPipelineState")
            return
        }

        // Get pipeline state from device
        guard let devicePtr = cmdList.device else { return }
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()

        if let metalPSO = device.pipelineStates[pso],
        let renderPipeline = metalPSO.renderPipeline {
            encoder.setRenderPipelineState(renderPipeline)
            print("[Metal] Pipeline state set: id=\(pso)")
        } else {
            print("[Metal] ERROR: Invalid pipeline state: id=\(pso)")
        }
    }
}

// MARK: - Vertex/Index Buffer Functions

@_cdecl("MetalCommandList_setVertexBuffer")
func MetalCommandList_setVertexBuffer(
    _ cmdListPtr: MetalCommandListPtr,
    _ slot: UInt32,
    _ bufferID: MetalBufferID,
    _ stride: UInt32,
    _ offset: UInt32
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else { return }
        guard let devicePtr = cmdList.device else { return }

        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()

        if let metalBuffer = device.buffers[bufferID]?.buffer {
            encoder.setVertexBuffer(
                metalBuffer,
                offset: Int(offset),
                index: Int(slot)
            )
            print("[Metal] Vertex buffer set: slot=\(slot), id=\(bufferID)")
        } else {
            print("[Metal] ERROR: Invalid vertex buffer id: \(bufferID)")
        }
    }
}

@_cdecl("MetalCommandList_setIndexBuffer")
func MetalCommandList_setIndexBuffer(
    _ cmdListPtr: MetalCommandListPtr,
    _ bufferID: MetalBufferID,
    _ format: RHIIndexFormat,
    _ offset: UInt32
) {
    // Metal doesn't have a setIndexBuffer command - it's used in drawIndexed
    // Store for later use (would need to extend MetalCommandList struct)
}

// MARK: - Constant Buffer Functions

@_cdecl("MetalCommandList_setConstantBuffer")
func MetalCommandList_setConstantBuffer(
    _ cmdListPtr: MetalCommandListPtr,
    _ slot: UInt32,
    _ bufferID: MetalBufferID,
    _ stage: RHIShaderStage
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else { return }
        guard let devicePtr = cmdList.device else { return }

        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()

        if let metalBuffer = device.buffers[bufferID]?.buffer {
            switch stage {
            case RHIShaderStage_Vertex:
                encoder.setVertexBuffer(
                    metalBuffer,
                    offset: 0,
                    index: Int(slot)
                )
            case RHIShaderStage_Pixel:
                encoder.setFragmentBuffer(
                    metalBuffer,
                    offset: 0,
                    index: Int(slot)
                )
            default:
                break
            }
        }
    }
}

// MARK: - Shader Resource Functions

@_cdecl("MetalCommandList_setTexture")
func MetalCommandList_setTexture(
    _ cmdListPtr: MetalCommandListPtr,
    _ slot: UInt32,
    _ textureID: MetalTextureID,
    _ stage: RHIShaderStage
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else { return }
        guard let devicePtr = cmdList.device else { return }

        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()

        if let metalTexture = device.textures[textureID]?.texture {
            switch stage {
            case RHIShaderStage_Vertex:
                encoder.setVertexTexture(
                    metalTexture,
                    index: Int(slot)
                )
                print("[Metal] Vertex texture set: slot=\(slot), id=\(textureID)")
            case RHIShaderStage_Pixel:
                encoder.setFragmentTexture(
                    metalTexture,
                    index: Int(slot)
                )
                print("[Metal] Fragment texture set: slot=\(slot), id=\(textureID)")
            default:
                break
            }
        } else {
            print("[Metal] ERROR: Invalid texture id: \(textureID)")
        }
    }
}

@_cdecl("MetalCommandList_setBuffer")
func MetalCommandList_setBuffer(
    _ cmdListPtr: MetalCommandListPtr,
    _ slot: UInt32,
    _ bufferID: MetalBufferID,
    _ stage: RHIShaderStage
) {
    // Same as setConstantBuffer
    MetalCommandList_setConstantBuffer(
        cmdListPtr,
        slot,
        bufferID,
        stage
    )
}

// MARK: - Viewport/Scissor Functions

@_cdecl("MetalCommandList_setViewport")
func MetalCommandList_setViewport(
    _ cmdListPtr: MetalCommandListPtr,
    _ viewportPtr: UnsafePointer<RHIViewport>?
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder,
            let vp = viewportPtr?.pointee else { return }

        let mtlViewport = MTLViewport(
            originX: Double(vp.x),
            originY: Double(vp.y),
            width: Double(vp.width),
            height: Double(vp.height),
            znear: Double(vp.minDepth),
            zfar: Double(vp.maxDepth)
        )

        encoder.setViewport(mtlViewport)
    }
}

@_cdecl("MetalCommandList_setViewports")
func MetalCommandList_setViewports(
    _ cmdListPtr: MetalCommandListPtr,
    _ viewportsPtr: UnsafePointer<RHIViewport>?,
    _ count: UInt32
) {
    autoreleasepool{
        // For now, just set the first viewport
        if count > 0 {
            MetalCommandList_setViewport(cmdListPtr, viewportsPtr)
        }
    }
}

@_cdecl("MetalCommandList_setScissorRect")
func MetalCommandList_setScissorRect(
    _ cmdListPtr: MetalCommandListPtr,
    _ scissorPtr: UnsafePointer<RHIScissorRect>?
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder,
            let scissor = scissorPtr?.pointee else { return }

        let mtlScissor = MTLScissorRect(
            x: Int(scissor.left),
            y: Int(scissor.top),
            width: Int(scissor.right - scissor.left),
            height: Int(scissor.bottom - scissor.top)
        )

        encoder.setScissorRect(mtlScissor)
    }
}

@_cdecl("MetalCommandList_setScissorRects")
func MetalCommandList_setScissorRects(
    _ cmdListPtr: MetalCommandListPtr,
    _ scissorsPtr: UnsafePointer<RHIScissorRect>?,
    _ count: UInt32
) {
    autoreleasepool{
        // For now, just set the first scissor rect
        if count > 0 {
            MetalCommandList_setScissorRect(cmdListPtr, scissorsPtr)
        }
    }
}

// MARK: - Draw Functions

@_cdecl("MetalCommandList_draw")
func MetalCommandList_draw(
    _ cmdListPtr: MetalCommandListPtr,
    _ vertexCount: UInt32,
    _ instanceCount: UInt32,
    _ startVertex: UInt32,
    _ startInstance: UInt32
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else {
            print("[Metal] No active render encoder for draw")
            return
        }

        encoder.drawPrimitives(
            type: .triangle,
            vertexStart: Int(startVertex),
            vertexCount: Int(vertexCount),
            instanceCount: Int(instanceCount),
            baseInstance: Int(startInstance)
        )
        print("[Metal] Draw call: vertices=\(vertexCount), instances=\(instanceCount)")
    }
}

@_cdecl("MetalCommandList_drawIndexed")
func MetalCommandList_drawIndexed(
    _ cmdListPtr: MetalCommandListPtr,
    _ indexCount: UInt32,
    _ instanceCount: UInt32,
    _ startIndex: UInt32,
    _ baseVertex: Int32,
    _ startInstance: UInt32
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        guard let encoder = cmdList.renderEncoder else {
            print("[Metal] No active render encoder for drawIndexed")
            return
        }

        // TODO: Need to track index buffer from setIndexBuffer
        // For now, this is a placeholder
        print("[Metal] drawIndexed not fully implemented yet")
    }
}

// MARK: - Compute Functions

@_cdecl("MetalCommandList_dispatch")
func MetalCommandList_dispatch(
    _ cmdListPtr: MetalCommandListPtr,
    _ threadGroupCountX: UInt32,
    _ threadGroupCountY: UInt32,
    _ threadGroupCountZ: UInt32
) {
    autoreleasepool{
        // TODO: Implement compute dispatch
        print("[Metal] Compute dispatch not yet implemented")
    }
}

// MARK: - Copy Functions

@_cdecl("MetalCommandList_copyBuffer")
func MetalCommandList_copyBuffer(
    _ cmdListPtr: MetalCommandListPtr,
    _ src: MetalBufferID,
    _ dst: MetalBufferID,
    _ srcOffset: Int,
    _ dstOffset: Int,
    _ size: Int
) {
    autoreleasepool{
        // TODO: Implement buffer copy using blit encoder
        print("[Metal] Buffer copy not yet implemented")
    }
}

@_cdecl("MetalCommandList_copyTexture")
func MetalCommandList_copyTexture(
    _ cmdListPtr: MetalCommandListPtr,
    _ src: MetalTextureID,
    _ dst: MetalTextureID
) {
    autoreleasepool{
        // TODO: Implement texture copy using blit encoder
        print("[Metal] Texture copy not yet implemented")
    }
}

@_cdecl("MetalCommandList_copyBufferToTexture")
func MetalCommandList_copyBufferToTexture(
    _ cmdListPtr: MetalCommandListPtr,
    _ src: MetalBufferID,
    _ dst: MetalTextureID,
    _ mipLevel: UInt32,
    _ arraySlice: UInt32
) {
    autoreleasepool{
        // TODO: Implement buffer-to-texture copy using blit encoder
        print("[Metal] Buffer-to-texture copy not yet implemented")
    }
}

// MARK: - Debug Marker Functions

@_cdecl("MetalCommandList_beginEvent")
func MetalCommandList_beginEvent(
    _ cmdListPtr: MetalCommandListPtr,
    _ name: UnsafePointer<CChar>
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()
        let eventName = String(cString: name)

        if let encoder = cmdList.renderEncoder {
            encoder.pushDebugGroup(eventName)
        }
    }
}

@_cdecl("MetalCommandList_endEvent")
func MetalCommandList_endEvent(
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        if let encoder = cmdList.renderEncoder {
            encoder.popDebugGroup()
        }
    }
}

@_cdecl("MetalCommandList_setMarker")
func MetalCommandList_setMarker(
    _ cmdListPtr: MetalCommandListPtr,
    _ name: UnsafePointer<CChar>
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()
        let markerName = String(cString: name)

        if let encoder = cmdList.renderEncoder {
            encoder.insertDebugSignpost(markerName)
        }
    }
}

// MARK: - Metal-Specific Functions

@_cdecl("MetalCommandList_setDrawable")
func MetalCommandList_setDrawable(
    _ cmdListPtr: MetalCommandListPtr,
    _ drawable: UnsafeMutableRawPointer
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        let metalDrawable = Unmanaged<CAMetalDrawable>
            .fromOpaque(drawable).takeUnretainedValue()
        cmdList.currentDrawable = metalDrawable
    }
}
