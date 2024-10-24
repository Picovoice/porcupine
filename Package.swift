// swift-tools-version:5.5
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
    targets: [
        .target(
            name: "Porcupine",
            path: "binding/ios",
            exclude: ["PorcupineAppTest"]
        )
    ],
    swiftLanguageVersions: [.v5]
)
