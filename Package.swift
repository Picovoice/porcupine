// swift-tools-version:5.3
import PackageDescription

let package = Package(
    name: "Porcupine-iOS",
    platforms: [
        .iOS(.v13)
    ],
    products: [
        .library(
            name: "Porcupine",
            targets: ["Porcupine"]
        )
    ],
    dependencies: [
        .package(
            url: "https://github.com/Picovoice/ios-voice-processor.git",
            .upToNextMajor(from: "1.1.2")
        )
    ],
    targets: [
        .binaryTarget(
            name: "PvPorcupine",
            path: "lib/ios/PvPorcupine.xcframework"
        ),
        .target(
            name: "Porcupine",
            dependencies: [
                "PvPorcupine",
                .product(name: "ios_voice_processor", package: "ios-voice-processor")
            ],
            path: "binding/ios",
            exclude: ["PorcupineAppTest"]
            resources: [
                .process("resources/keyword_files/ios/alexa_ios.ppn"),
                .process("resources/keyword_files/ios/americano_ios.ppn"),
                .process("resources/keyword_files/ios/blueberry_ios.ppn"),
                .process("resources/keyword_files/ios/bumblebee_ios.ppn"),
                .process("resources/keyword_files/ios/computer_ios.ppn"),
                .process("resources/keyword_files/ios/grapefruit_ios.ppn"),
                .process("resources/keyword_files/ios/grasshopper_ios.ppn"),
                // .process("resources/keyword_files/ios/hey\ google_ios.ppn"),
                // .process("resources/keyword_files/ios/hey\ siri_ios.ppn"),
                .process("resources/keyword_files/ios/jarvis_ios.ppn"),
                // .process("resources/keyword_files/ios/ok\ google_ios.ppn"),
                .process("resources/keyword_files/ios/picovoice_ios.ppn"),
                .process("resources/keyword_files/ios/porcupine_ios.ppn"),
                .process("resources/keyword_files/ios/terminator_ios.ppn"),
                .process("lib/common/porcupine_params.pv")
            ]
        )
    ]
)
