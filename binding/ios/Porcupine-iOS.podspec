Pod::Spec.new do |s|
  s.name = 'Porcupine-iOS'
  s.module_name = 'Porcupine'
  s.version = '2.2.0'
  s.license = {:type => 'Apache 2.0'}
  s.summary = 'iOS SDK for Picovoice\'s Porcupine wake word engine'
  s.description =
  <<-DESC
  Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled applications.

  Porcupine is:
    - using deep neural networks trained in real-world environments.
    - compact and computationally-efficient. It is perfect for IoT.
    - cross-platform:
      - Arm Cortex-M, STM32, Arduino, and i.MX RT
      - Raspberry Pi, NVIDIA Jetson Nano, and BeagleBone
      - Android and iOS
      - Chrome, Safari, Firefox, and Edge
      - Linux (x86_64), macOS (x86_64, arm64), and Windows (x86_64)
    - scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
    - self-service. Developers can train custom wake word models using [Picovoice Console](https://picovoice.ai/console/).
  DESC
  s.homepage = 'https://github.com/Picovoice/porcupine/tree/master/binding/ios'
  s.author = { 'Picovoice' => 'hello@picovoice.ai' }
  s.source = { :git => "https://github.com/Picovoice/porcupine.git", :tag => "Porcupine-iOS-v2.2.0" }
  s.ios.deployment_target = '11.0'
  s.swift_version = '5.0'
  s.vendored_frameworks = 'lib/ios/PvPorcupine.xcframework'
  s.resource_bundles = {
    'PorcupineResources' => [
      'resources/keyword_files/ios/{alexa,americano,blueberry,bumblebee,computer,grapefruit,grasshopper,hey google,hey siri,jarvis,ok google,picovoice,porcupine,terminator}_ios.ppn',
      'lib/common/porcupine_params.pv'
    ]
  }
  s.source_files = 'binding/ios/*.{swift}'
  s.exclude_files = 'binding/ios/PorcupineAppTest/**'

  s.dependency 'ios-voice-processor', '~> 1.0.2'
end
