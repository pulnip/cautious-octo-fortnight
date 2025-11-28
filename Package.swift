// swift-tools-version: 6.1
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "RenderToy",
    platforms: [
        .macOS(.v15),
    ],
    targets: [
        .target(
            name: "RenderToyRHI",
            dependencies: [],
            path: "engine/src/RHI/Metal",
            exclude: [
                "MetalCommandList_CPPWrapper.cpp",
                "MetalDevice_CPPWrapper.cpp",
                "MetalDeviceFactory.cpp",
                "MetalCommandList_Bridge.h",
                "MetalDevice_Bridge.h",
                "MetalTypes.h",
                "MetalCommandList_CPPWrapper.hpp",
                "MetalDevice_CPPWrapper.hpp"
            ],
            publicHeadersPath: nil,
            swiftSettings: [
                .enableExperimentalFeature("StrictConcurrency"),
                .unsafeFlags(
                    [
                        "-import-objc-header",
                        "RenderToyMetalRHI-Bridging-Header.h",
                    ]
                )
            ],
            linkerSettings: [
                .linkedFramework("Metal"),
                .linkedFramework("MetalKit"),
                .linkedFramework("QuartzCore"),
                .linkedFramework("Foundation")
            ]
        ),
    ],
)
