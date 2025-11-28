import Metal
import QuartzCore
import Foundation

// MARK: - Metal Resource Classes

class MetalBuffer {
    var buffer: MTLBuffer?
    var size: Int = 0
    var usage: RHIBufferUsageFlags = BufNone
    var isCPUAccessible: Bool = false

    func cleanup() {
        buffer = nil
    }

    func upload(
        data: UnsafeRawPointer,
        size: Int,
        offset: Int
    ) {
        guard let buffer = buffer else { return }
        guard isCPUAccessible else {
            print("[Metal] Direct upload to GPU-only buffer not yet supported")
            return
        }

        let contents = buffer.contents()
        memcpy(contents.advanced(by: offset), data, size)

        #if os(macOS)
        buffer.didModifyRange(offset..<(offset + size))
        #endif
    }
}

class MetalTexture {
    var texture: MTLTexture?
    var width: UInt32 = 0
    var height: UInt32 = 0
    var format: RHITextureFormat = Unknown

    func cleanup() {
        texture = nil
    }
}

class MetalShader {
    var function: MTLFunction?
    var library: MTLLibrary?
    var stage: RHIShaderStage = RHIShaderStage_Vertex

    func cleanup() {
        function = nil
        library = nil
    }
}

class MetalPipelineState {
    var renderPipeline: MTLRenderPipelineState?
    var computePipeline: MTLComputePipelineState?

    func cleanup() {
        renderPipeline = nil
        computePipeline = nil
    }
}

class MetalSwapchain {
    var metalLayer: CAMetalLayer?
    var width: UInt32 = 0
    var height: UInt32 = 0
    var currentDrawable: CAMetalDrawable?

    func cleanup() {
        currentDrawable = nil
        metalLayer = nil
    }
}

class MetalFence {
    var value: UInt64 = 0

    func cleanup() {
        // No-op for now
    }
}

// MARK: - Main MetalDevice Class

class MetalDevice{
    var device: MTLDevice?
    var commandQueue: MTLCommandQueue?
    var deviceName: String = ""

    // Resource storage
    var buffers: [MetalBufferID: MetalBuffer] = [:]
    var textures: [MetalTextureID: MetalTexture] = [:]
    var shaders: [MetalShaderID: MetalShader] = [:]
    var pipelineStates: [MetalPipelineStateID: MetalPipelineState] = [:]
    var swapchains: [MetalSwapchainID: MetalSwapchain] = [:]
    var fences: [MetalFenceID: MetalFence] = [:]

    // ID counters
    var nextBufferId: MetalBufferID = 1
    var nextTextureId: MetalTextureID = 1
    var nextShaderId: MetalShaderID = 1
    var nextPipelineStateId: MetalPipelineStateID = 1
    var nextSwapchainId: MetalSwapchainID = 1
    var nextFenceId: MetalFenceID = 1

    func cleanup(){
        // Wait for idle (Metal doesn't have direct waitUntilIdle, handled by synchronization)
        // TODO: Add proper synchronization if needed

        // Clean up all resources
        for(_, buffer) in buffers{ buffer.cleanup() }
        for(_, texture) in textures{ texture.cleanup() }
        for(_, shader) in shaders{ shader.cleanup() }
        for(_, pso) in pipelineStates{ pso.cleanup() }
        for(_, swapchain) in swapchains{ swapchain.cleanup() }
        for(_, fence) in fences{ fence.cleanup() }

        buffers.removeAll()
        textures.removeAll()
        shaders.removeAll()
        pipelineStates.removeAll()
        swapchains.removeAll()
        fences.removeAll()

        commandQueue = nil
        device = nil
    }

    deinit{
        cleanup()
    }
}

// MARK: - C Bridge Functions

@_cdecl("MetalDevice_create")
func MetalDevice_create(
    _ desc: UnsafePointer<RHIDeviceCreateDesc>
) -> MetalDevicePtr? {
    autoreleasepool{
        let metalDevice = MetalDevice()

        // Get default Metal device
        guard let device = MTLCreateSystemDefaultDevice() else {
            print("[Metal] Failed to create Metal device - no Metal-capable GPU found")
            return nil
        }

        metalDevice.device = device
        metalDevice.deviceName = device.name

        // Create command queue
        guard let commandQueue = device.makeCommandQueue() else {
            print("[Metal] Failed to create Metal command queue")
            return nil
        }

        metalDevice.commandQueue = commandQueue
        commandQueue.label = "RenderToy Main Command Queue"

        print("[Metal] Metal device initialized: \(metalDevice.deviceName)")

        return Unmanaged.passRetained(metalDevice).toOpaque()
    }
}

@_cdecl("MetalDevice_destroy")
func MetalDevice_destroy(_ devicePtr: MetalDevicePtr) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeRetainedValue()
        device.cleanup()
    }
}

@_cdecl("MetalDevice_getDeviceName")
func MetalDevice_getDeviceName(
    _ devicePtr: MetalDevicePtr
) -> UnsafePointer<CChar>? {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        return (device.deviceName as NSString).utf8String
    }
}

@_cdecl("MetalDevice_getAPIName")
func MetalDevice_getAPIName(
    _ devicePtr: MetalDevicePtr
) -> UnsafePointer<CChar> {
    autoreleasepool{
        return ("Metal" as NSString).utf8String!
    }
}

// MARK: - Buffer Functions

@_cdecl("MetalDevice_createBuffer")
func MetalDevice_createBuffer(
    _ devicePtr: MetalDevicePtr,
    _ descPtr: UnsafePointer<RHIBufferCreateDesc>?
) -> MetalBufferID {
    autoreleasepool{
        let metalDevice = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let device = metalDevice.device,
            let desc = descPtr?.pointee else {
            return 0
        }

        let metalBuffer = MetalBuffer()
        metalBuffer.size = desc.size
        metalBuffer.usage = desc.usage

        // Determine CPU accessibility
        let hasVertexUsage = (desc.usage.rawValue & BufVertexBuffer.rawValue) != 0
        let hasIndexUsage = (desc.usage.rawValue & BufIndexBuffer.rawValue) != 0
        let hasConstantUsage = (desc.usage.rawValue & BufConstantBuffer.rawValue) != 0
        metalBuffer.isCPUAccessible = hasVertexUsage || hasIndexUsage || hasConstantUsage || desc.initialData != nil

        // Determine resource options
        var options: MTLResourceOptions
        if metalBuffer.isCPUAccessible {
            #if os(macOS)
            options = .storageModeManaged
            #else
            options = .storageModeShared
            #endif
        } else {
            options = .storageModePrivate
        }

        // Create buffer
        guard let buffer = device.makeBuffer(
            length: desc.size,
            options: options
        ) else {
            print("[Metal] Failed to create buffer of size \(desc.size)")
            return 0
        }

        metalBuffer.buffer = buffer

        // Upload initial data
        if let initialData = desc.initialData, metalBuffer.isCPUAccessible {
            metalBuffer.upload(
                data: initialData,
                size: desc.size,
                offset: 0
            )
        }

        let id = metalDevice.nextBufferId
        metalDevice.nextBufferId += 1
        metalDevice.buffers[id] = metalBuffer

        print("[Metal] Created buffer: id=\(id), size=\(desc.size)")
        return id
    }
}

@_cdecl("MetalDevice_destroyBuffer")
func MetalDevice_destroyBuffer(
    _ devicePtr: MetalDevicePtr,
    _ handle: MetalBufferID
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let buffer = device.buffers[handle] {
            buffer.cleanup()
            device.buffers.removeValue(forKey: handle)
        }
    }
}

@_cdecl("MetalDevice_uploadBufferData")
func MetalDevice_uploadBufferData(
    _ devicePtr: MetalDevicePtr,
    _ bufferID: MetalBufferID,
    _ data: UnsafeRawPointer,
    _ size: Int,
    _ offset: Int
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let buffer = device.buffers[bufferID] else { return }
        buffer.upload(
            data: data,
            size: size,
            offset: offset
        )
    }
}

@_cdecl("MetalDevice_updateBuffer")
func MetalDevice_updateBuffer(
    _ devicePtr: MetalDevicePtr,
    _ bufferID: MetalBufferID,
    _ data: UnsafeRawPointer,
    _ size: Int,
    _ offset: Int
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let buffer = device.buffers[bufferID] else { return }
        buffer.upload(
            data: data,
            size: size,
            offset: offset
        )
    }
}

// MARK: - Texture Functions

@_cdecl("MetalDevice_createTexture")
func MetalDevice_createTexture(
    _ devicePtr: MetalDevicePtr,
    _ descPtr: UnsafePointer<RHITextureCreateDesc>?
) -> MetalTextureID {
    autoreleasepool{
        let metalDevice = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let device = metalDevice.device,
            let desc = descPtr?.pointee else { return 0 }

        print("[Metal] Creating texture: size=\(desc.width)x\(desc.height), format=\(desc.format)")

        let textureDesc = MTLTextureDescriptor()
        textureDesc.width = Int(desc.width)
        textureDesc.height = Int(desc.height)
        textureDesc.depth = Int(desc.depth)
        textureDesc.mipmapLevelCount = Int(desc.mipLevels)
        textureDesc.arrayLength = Int(desc.arraySize)

        let mtlFormat = convertTextureFormat(desc.format)
        print("[Metal] Converted format \(desc.format) -> MTLPixelFormat \(mtlFormat.rawValue)")
        textureDesc.pixelFormat = mtlFormat
        textureDesc.textureType = desc.depth > 1 ? .type3D : (desc.arraySize > 1 ? .type2DArray : .type2D)
        textureDesc.usage = convertTextureUsage(desc.usage)
        // Use .shared to allow CPU writes via replace()
        textureDesc.storageMode = .shared

        guard let texture = device.makeTexture(descriptor: textureDesc) else {
            print("[Metal] Failed to create texture \(desc.width)x\(desc.height)")
            return 0
        }

        let metalTexture = MetalTexture()
        metalTexture.texture = texture
        metalTexture.width = desc.width
        metalTexture.height = desc.height
        metalTexture.format = desc.format

        let id = metalDevice.nextTextureId
        metalDevice.nextTextureId += 1
        metalDevice.textures[id] = metalTexture

        print("[Metal] Created texture: id=\(id), size=\(desc.width)x\(desc.height)")
        return id
    }
}

@_cdecl("MetalDevice_destroyTexture")
func MetalDevice_destroyTexture(
    _ devicePtr: MetalDevicePtr,
    _ textureID: MetalTextureID
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let texture = device.textures[textureID] {
            texture.cleanup()
            device.textures.removeValue(
                forKey: textureID
            )
        }
    }
}

@_cdecl("MetalDevice_uploadTextureData")
func MetalDevice_uploadTextureData(
    _ devicePtr: MetalDevicePtr,
    _ textureID: MetalTextureID,
    _ data: UnsafeRawPointer,
    _ dataSize: Int,
    _ mipLevel: UInt32,
    _ arraySlice: UInt32
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let metalTexture = device.textures[textureID],
            let texture = metalTexture.texture else { return }

        let bytesPerPixel = getBytesPerPixel(metalTexture.format)
        let bytesPerRow = Int(metalTexture.width) * bytesPerPixel
        let region = MTLRegionMake2D(0, 0, Int(metalTexture.width), Int(metalTexture.height))

        texture.replace(
            region: region,
            mipmapLevel: Int(mipLevel),
            slice: Int(arraySlice),
            withBytes: data,
            bytesPerRow: bytesPerRow,
            bytesPerImage: 0
        )
    }
}

// MARK: - Shader Functions

@_cdecl("MetalDevice_createShader")
func MetalDevice_createShader(
    _ devicePtr: MetalDevicePtr,
    _ descPtr: UnsafePointer<RHIShaderCreateDesc>?
) -> MetalShaderID {
    autoreleasepool{
        let metalDevice = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let device = metalDevice.device,
            let desc = descPtr?.pointee else { return 0 }

        // Shader source is MSL text
        guard let source = desc.bytecode else {
            print("[Metal] Shader bytecode is null")
            return 0
        }

        let sourceString = String(cString: source.assumingMemoryBound(to: CChar.self))

        do {
            let library = try device.makeLibrary(source: sourceString, options: nil)
            guard let entryPointName = desc.entryPoint,
                let entryPoint = String.init(validatingCString: entryPointName),
                let function = library.makeFunction(name: entryPoint) else {
                print("[Metal] Failed to find shader entry point")
                return 0
            }

            let metalShader = MetalShader()
            metalShader.library = library
            metalShader.function = function
            metalShader.stage = desc.stage

            let id = metalDevice.nextShaderId
            metalDevice.nextShaderId += 1
            metalDevice.shaders[id] = metalShader

            print("[Metal] Created shader: id=\(id)")
            return id
        } catch {
            print("[Metal] Failed to create shader library: \(error)")
            return 0
        }
    }
}

@_cdecl("MetalDevice_destroyShader")
func MetalDevice_destroyShader(
    _ devicePtr: MetalDevicePtr,
    _ shaderID: MetalShaderID
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let shader = device.shaders[shaderID] {
            shader.cleanup()
            device.shaders.removeValue(
                forKey: shaderID
            )
        }
    }
}

// MARK: - Pipeline State Functions

@_cdecl("MetalDevice_createGraphicsPipelineState")
func MetalDevice_createGraphicsPipelineState(
    _ devicePtr: MetalDevicePtr,
    _ descPtr: UnsafePointer<RHIGraphicsPipelineStateDesc>?
) -> MetalPipelineStateID {
    autoreleasepool{
        let metalDevice = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let device = metalDevice.device,
            let desc = descPtr?.pointee else { return 0 }

        let vertexHandle = desc.vertexShaderIndex
        let pixelHandle = desc.pixelShaderIndex

        print("[Metal] Creating pipeline with vertex shader=\(vertexHandle), pixel shader=\(pixelHandle)")
        print("[Metal] Available shaders: \(metalDevice.shaders.keys.sorted())")

        guard let vertexShader = metalDevice.shaders[vertexHandle]?.function,
            let pixelShader = metalDevice.shaders[pixelHandle]?.function else {
            print("[Metal] Invalid shader handles for pipeline (vertex=\(vertexHandle), pixel=\(pixelHandle))")
            return 0
        }

        let pipelineDesc = MTLRenderPipelineDescriptor()
        pipelineDesc.vertexFunction = vertexShader
        pipelineDesc.fragmentFunction = pixelShader

        // Set render target formats
        let formatArray = [
            desc.renderTargetFormats.0,
            desc.renderTargetFormats.1,
            desc.renderTargetFormats.2,
            desc.renderTargetFormats.3,
            desc.renderTargetFormats.4,
            desc.renderTargetFormats.5,
            desc.renderTargetFormats.6,
            desc.renderTargetFormats.7
        ]
        for i in 0..<Int(desc.renderTargetCount) {
            pipelineDesc.colorAttachments[i].pixelFormat = convertTextureFormat(formatArray[i])
        }

        // Create vertex descriptor from layout
        pipelineDesc.vertexDescriptor = createVertexDescriptor(desc.vertexLayout)

        do {
            let pipelineState = try device.makeRenderPipelineState(descriptor: pipelineDesc)

            let metalPSO = MetalPipelineState()
            metalPSO.renderPipeline = pipelineState

            let id = metalDevice.nextPipelineStateId
            metalDevice.nextPipelineStateId += 1
            metalDevice.pipelineStates[id] = metalPSO

            print("[Metal] Created graphics pipeline: id=\(id)")
            return id
        } catch {
            print("[Metal] Failed to create pipeline state: \(error)")
            return 0
        }
    }
}

@_cdecl("MetalDevice_createComputePipelineState")
func MetalDevice_createComputePipelineState(
    _ devicePtr: MetalDevicePtr,
    _ descPtr: UnsafeRawPointer
) -> MetalPipelineStateID {
    autoreleasepool{
        // TODO: Implement compute pipeline
        print("[Metal] Compute pipelines not yet implemented")
        return 0
    }
}

@_cdecl("MetalDevice_destroyPipelineState")
func MetalDevice_destroyPipelineState(
    _ devicePtr: MetalDevicePtr,
    _ pipelineID: MetalPipelineStateID
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let pso = device.pipelineStates[pipelineID] {
            pso.cleanup()
            device.pipelineStates.removeValue(forKey: pipelineID)
        }
    }
}

// MARK: - Swapchain Functions

@_cdecl("MetalDevice_createSwapchain")
func MetalDevice_createSwapchain(
    _ devicePtr: MetalDevicePtr,
    _ descPtr: UnsafePointer<RHISwapchainCreateDesc>?
) -> MetalSwapchainID {
    autoreleasepool{
        let metalDevice = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let desc = descPtr?.pointee else{ return 0 }

        print("[Metal] Creating swapchain: size=\(desc.width)x\(desc.height), format=\(desc.format)")

        // windowHandle should be a CAMetalLayer pointer
        guard let layerPtr = desc.windowHandle else {
            print("[Metal] Swapchain window handle is null")
            return 0
        }

        let layer = Unmanaged<CAMetalLayer>
            .fromOpaque(layerPtr).takeUnretainedValue()
        layer.device = metalDevice.device

        let mtlFormat = convertTextureFormat(desc.format)
        print("[Metal] Converted swapchain format \(desc.format) -> MTLPixelFormat \(mtlFormat.rawValue)")
        layer.pixelFormat = mtlFormat
        layer.framebufferOnly = true
        layer.drawableSize = CGSize(width: Int(desc.width), height: Int(desc.height))

        let swapchain = MetalSwapchain()
        swapchain.metalLayer = layer
        swapchain.width = desc.width
        swapchain.height = desc.height

        let id = metalDevice.nextSwapchainId
        metalDevice.nextSwapchainId += 1
        metalDevice.swapchains[id] = swapchain

        print("[Metal] Created swapchain: id=\(id), size=\(desc.width)x\(desc.height)")
        return id
    }
}

@_cdecl("MetalDevice_destroySwapchain")
func MetalDevice_destroySwapchain(
    _ devicePtr: MetalDevicePtr,
    _ swapchainID: MetalSwapchainID
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let swapchain = device.swapchains[swapchainID] {
            swapchain.cleanup()
            device.swapchains.removeValue(forKey: swapchainID)
        }
    }
}

@_cdecl("MetalDevice_resizeSwapchain")
func MetalDevice_resizeSwapchain(
    _ devicePtr: MetalDevicePtr,
    _ swapchainID: MetalSwapchainID,
    _ width: UInt32,
    _ height: UInt32
) -> Bool {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let swapchain = device.swapchains[swapchainID],
            let layer = swapchain.metalLayer else {
            return false
        }

        layer.drawableSize = CGSize(width: Int(width), height: Int(height))
        swapchain.width = width
        swapchain.height = height
        return true
    }
}

@_cdecl("MetalDevice_present")
func MetalDevice_present(
    _ devicePtr: MetalDevicePtr,
    _ swapchainID: MetalSwapchainID
) -> Bool {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let swapchain = device.swapchains[swapchainID],
            let layer = swapchain.metalLayer else {
            return false
        }

        // Acquire next drawable
        if let drawable = layer.nextDrawable() {
            swapchain.currentDrawable = drawable
            return true
        }
        return false
    }
}

@_cdecl("MetalDevice_getSwapchainBackbuffer")
func MetalDevice_getSwapchainBackbuffer(
    _ devicePtr: MetalDevicePtr,
    _ swapchainID: MetalSwapchainID
) -> MetalTextureID {
    autoreleasepool{
        let metalDevice = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()

        guard let swapchain = metalDevice.swapchains[swapchainID],
            let drawable = swapchain.currentDrawable else {
            print("[Metal] No current drawable for swapchain \(swapchainID)")
            return 0
        }

        // Wrap the drawable's texture in a MetalTexture
        let metalTexture = MetalTexture()
        metalTexture.texture = drawable.texture
        metalTexture.width = UInt32(drawable.texture.width)
        metalTexture.height = UInt32(drawable.texture.height)
        metalTexture.format = BGRA8_UNORM // Metal drawables are always BGRA8

        let id = metalDevice.nextTextureId
        metalDevice.nextTextureId += 1
        metalDevice.textures[id] = metalTexture

        print("[Metal] Created backbuffer texture wrapper: id=\(id), size=\(metalTexture.width)x\(metalTexture.height)")
        return id
    }
}

@_cdecl("MetalDevice_getSwapchainCurrentIndex")
func MetalDevice_getSwapchainCurrentIndex(
    _ devicePtr: MetalDevicePtr,
    _ swapchainID: MetalSwapchainID
) -> UInt32 {
    autoreleasepool{
        return 0
    }
}

@_cdecl("MetalDevice_getSwapchainDrawable")
func MetalDevice_getSwapchainDrawable(
    _ devicePtr: MetalDevicePtr,
    _ swapchainID: MetalSwapchainID
) -> UnsafeMutableRawPointer? {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let swapchain = device.swapchains[swapchainID],
            let drawable = swapchain.currentDrawable else {
            return nil
        }

        return Unmanaged.passUnretained(drawable).toOpaque()
    }
}

// MARK: - Fence Functions (simple implementation)

@_cdecl("MetalDevice_createFence")
func MetalDevice_createFence(
    _ devicePtr: MetalDevicePtr,
    _ initialValue: UInt64
) -> MetalFenceID {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()

        let fence = MetalFence()
        fence.value = initialValue

        let id = device.nextFenceId
        device.nextFenceId += 1
        device.fences[id] = fence

        return id
    }
}

@_cdecl("MetalDevice_destroyFence")
func MetalDevice_destroyFence(
    _ devicePtr: MetalDevicePtr,
    _ fenceID: MetalFenceID
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let fence = device.fences[fenceID] {
            fence.cleanup()
            device.fences.removeValue(forKey: fenceID)
        }
    }
}

@_cdecl("MetalDevice_waitForFence")
func MetalDevice_waitForFence(
    _ devicePtr: MetalDevicePtr,
    _ fenceID: MetalFenceID,
    _ value: UInt64
) {
    autoreleasepool{
        // TODO: Implement with MTLSharedEvent
        // For now, this is a no-op
    }
}

@_cdecl("MetalDevice_signalFence")
func MetalDevice_signalFence(
    _ devicePtr: MetalDevicePtr,
    _ fenceID: MetalFenceID,
    _ value: UInt64
) {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        if let fence = device.fences[fenceID] {
            fence.value = value
        }
    }
}

@_cdecl("MetalDevice_getFenceValue")
func MetalDevice_getFenceValue(
    _ devicePtr: MetalDevicePtr,
    _ fenceID: MetalFenceID
) -> UInt64 {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        return device.fences[fenceID]?.value ?? 0
    }
}

@_cdecl("MetalDevice_isFenceComplete")
func MetalDevice_isFenceComplete(
    _ devicePtr: MetalDevicePtr,
    _ fenceID: MetalFenceID,
    _ value: UInt64
) -> Bool {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let fence = device.fences[fenceID] else { return false }
        return fence.value >= value
    }
}

// MARK: - Command List Functions

@_cdecl("MetalDevice_beginCommandList")
func MetalDevice_beginCommandList(
    _ devicePtr: MetalDevicePtr
) -> MetalCommandListPtr? {
    autoreleasepool{
        let device = Unmanaged<MetalDevice>
            .fromOpaque(devicePtr).takeUnretainedValue()
        guard let commandQueue = device.commandQueue else { return nil }

        // Create command queue pointer (retain for Swift)
        let queuePtr = Unmanaged.passUnretained(commandQueue).toOpaque()

        // Create MetalCommandList via its C bridge function
        guard let cmdListPtr = MetalCommandList_create(queuePtr, devicePtr) else {
            return nil
        }

        // Begin recording on the command list
        MetalCommandList_begin(cmdListPtr)

        return cmdListPtr
    }
}

@_cdecl("MetalDevice_submitCommandList")
func MetalDevice_submitCommandList(
    _ devicePtr: MetalDevicePtr,
    _ cmdListPtr: MetalCommandListPtr
) {
    autoreleasepool{
        let cmdList = Unmanaged<MetalCommandList>
            .fromOpaque(cmdListPtr).takeUnretainedValue()

        // End any active encoder
        if let encoder = cmdList.renderEncoder {
            encoder.endEncoding()
            cmdList.renderEncoder = nil
        }

        // Present the drawable if we have one
        if let drawable = cmdList.currentDrawable {
            cmdList.commandBuffer?.present(drawable)
        }

        // Commit the command buffer
        cmdList.commandBuffer?.commit()

        // Clean up
        cmdList.cleanup()
        print("[Metal] Command list submitted and presented")
    }
}

@_cdecl("MetalDevice_submitCommandListWithFence")
func MetalDevice_submitCommandListWithFence(
    _ devicePtr: MetalDevicePtr,
    _ cmdListPtr: MetalCommandListPtr,
    _ fenceID: MetalFenceID,
    _ signalValue: UInt64
) {
    autoreleasepool{
        // TODO: Implement command list submission with fence
    }
}

// MARK: - Synchronization Functions

@_cdecl("MetalDevice_waitForIdle")
func MetalDevice_waitForIdle(
    _ devicePtr: MetalDevicePtr
) {
    autoreleasepool{
        // TODO: Implement proper wait for idle
        // For now, this is a no-op
    }
}

// MARK: - Helper Functions

func convertTextureFormat(_ format: RHITextureFormat) -> MTLPixelFormat {
    // RHITextureFormat enum values (must match RHIDefinitions.hpp)
    switch format {
    case Unknown: return .invalid

    // 8-bit formats
    case R8_UNORM: return .r8Unorm
    case R8_SNORM: return .r8Snorm
    case R8_UINT: return .r8Uint
    case R8_SINT: return .r8Sint

    // 16-bit formats
    case R16_UNORM: return .r16Unorm
    case R16_SNORM: return .r16Snorm
    case R16_UINT: return .r16Uint
    case R16_SINT: return .r16Sint
    case R16_FLOAT: return .r16Float

    case RG8_UNORM: return .rg8Unorm
    case RG8_SNORM: return .rg8Snorm
    case RG8_UINT: return .rg8Uint
    case RG8_SINT: return .rg8Sint

    // 32-bit formats
    case R32_UINT: return .r32Uint
    case R32_SINT: return .r32Sint
    case R32_FLOAT: return .r32Float

    case RG16_UNORM: return .rg16Unorm
    case RG16_SNORM: return .rg16Snorm
    case RG16_UINT: return .rg16Uint
    case RG16_SINT: return .rg16Sint
    case RG16_FLOAT: return .rg16Float

    case RGBA8_UNORM: return .rgba8Unorm
    case RGBA8_UNORM_SRGB: return .rgba8Unorm_srgb
    case RGBA8_SNORM: return .rgba8Snorm
    case RGBA8_UINT: return .rgba8Uint
    case RGBA8_SINT: return .rgba8Sint

    case BGRA8_UNORM: return .bgra8Unorm
    case BGRA8_UNORM_SRGB: return .bgra8Unorm_srgb

    // 64-bit formats
    case RG32_UINT: return .rg32Uint
    case RG32_SINT: return .rg32Sint
    case RG32_FLOAT: return .rg32Float

    case RGBA16_UNORM: return .rgba16Unorm
    case RGBA16_SNORM: return .rgba16Snorm
    case RGBA16_UINT: return .rgba16Uint
    case RGBA16_SINT: return .rgba16Sint
    case RGBA16_FLOAT: return .rgba16Float

    // 128-bit formats
    case RGBA32_UINT: return .rgba32Uint
    case RGBA32_SINT: return .rgba32Sint
    case RGBA32_FLOAT: return .rgba32Float

    // Depth/stencil formats
    case D16_UNORM: return .depth16Unorm
    case D24_UNORM_S8_UINT: return .depth24Unorm_stencil8
    case D32_FLOAT: return .depth32Float
    case D32_FLOAT_S8_UINT: return .depth32Float_stencil8

    default: return .invalid
    }
}

func convertTextureUsage(_ usage: RHITextureUsageFlags) -> MTLTextureUsage {
    var mtlUsage: MTLTextureUsage = []

    if (usage.rawValue & TexShaderResource.rawValue) != 0 { mtlUsage.insert(.shaderRead) }
    if (usage.rawValue & TexRenderTarget.rawValue) != 0 { mtlUsage.insert(.renderTarget) }
    if (usage.rawValue & TexDepthStencil.rawValue) != 0 { mtlUsage.insert(.renderTarget) }
    if (usage.rawValue & TexUnorderedAccess.rawValue) != 0 { mtlUsage.insert(.shaderWrite) }

    return mtlUsage
}

func getBytesPerPixel(_ format: RHITextureFormat) -> Int {
    switch format {
    case R8_UNORM, R8_SNORM, R8_UINT, R8_SINT:
        return 1
    case R16_UNORM, R16_SNORM, R16_UINT, R16_SINT, R16_FLOAT,
         RG8_UNORM, RG8_SNORM, RG8_UINT, RG8_SINT:
        return 2
    case R32_UINT, R32_SINT, R32_FLOAT,
         RG16_UNORM, RG16_SNORM, RG16_UINT, RG16_SINT, RG16_FLOAT,
         RGBA8_UNORM, RGBA8_UNORM_SRGB, RGBA8_SNORM, RGBA8_UINT, RGBA8_SINT,
         BGRA8_UNORM, BGRA8_UNORM_SRGB:
        return 4
    case RG32_UINT, RG32_SINT, RG32_FLOAT,
         RGBA16_UNORM, RGBA16_SNORM, RGBA16_UINT, RGBA16_SINT, RGBA16_FLOAT:
        return 8
    case RGBA32_UINT, RGBA32_SINT, RGBA32_FLOAT:
        return 16
    case D16_UNORM:
        return 2
    case D24_UNORM_S8_UINT, D32_FLOAT:
        return 4
    case D32_FLOAT_S8_UINT:
        return 8
    default:
        return 4
    }
}

func createVertexDescriptor(_ layout: RHIVertexLayout) -> MTLVertexDescriptor? {
    guard layout.elementCount > 0, let elements = layout.elements else {
        return nil
    }

    let vertexDesc = MTLVertexDescriptor()

    for i in 0..<Int(layout.elementCount) {
        let element = elements[Int(i)]
        guard let attr = vertexDesc.attributes[Int(i)] else { continue }
        attr.format = convertVertexFormat(element.format)
        attr.offset = Int(element.alignedByteOffset)
        attr.bufferIndex = Int(element.inputSlot)
    }

    // Set up buffer layout (assume tightly packed for now)
    vertexDesc.layouts[0].stride = calculateVertexStride(layout)
    vertexDesc.layouts[0].stepFunction = .perVertex

    return vertexDesc
}

func convertVertexFormat(_ format: RHITextureFormat) -> MTLVertexFormat {
    // RHITextureFormat enum values (must match RHIDefinitions.hpp)
    switch format {
    case R32_FLOAT: return .float
    case RG32_FLOAT: return .float2
    case RGBA32_FLOAT: return .float4
    default: return .invalid
    }
}

func calculateVertexStride(_ layout: RHIVertexLayout) -> Int {
    guard layout.elementCount > 0, let elements = layout.elements else {
        return 0
    }

    var maxOffset = 0
    var lastSize = 0

    for i in 0..<Int(layout.elementCount) {
        let element = elements[Int(i)]
        let offset = Int(element.alignedByteOffset)
        let size = getVertexFormatSize(element.format)

        if offset >= maxOffset {
            maxOffset = offset
            lastSize = size
        }
    }

    return maxOffset + lastSize
}

func getVertexFormatSize(_ format: RHITextureFormat) -> Int {
    switch format {
    case R32_FLOAT: return 4
    case RG32_FLOAT: return 8
    case RGBA32_FLOAT: return 16
    default: return 0
    }
}
