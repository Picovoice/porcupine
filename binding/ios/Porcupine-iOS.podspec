Pod::Spec.new do |s|
  s.name = 'Porcupine-iOS'
  s.module_name = 'Porcupine'
  s.version = '1.9.4'
  s.license = {:type => 'Apache 2.0'}
  s.summary = 'iOS SDK for Picovoice\'s Porcupine wake word engine'
  s.description = 
  <<-DESC
  Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled applications. 
  
  Porcupine is:
    * using deep neural networks trained in real-world environments.
    * compact and computationally-efficient. It is perfect for IoT.
    * cross-platform. Raspberry Pi, BeagleBone, Android, iOS, Linux (x86_64), macOS (x86_64), Windows (x86_64), and web browsers are supported. Additionally, enterprise customers have access to ARM Cortex-M SDK.
    * scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
    * self-service. Developers can train custom wake word models using Picovoice Console.
  DESC
  s.homepage = 'https://github.com/Picovoice/porcupine/tree/master/binding/ios'
  s.author = { 'Picovoice' => 'hello@picovoice.ai' }
  s.source = { :git => "https://github.com/Picovoice/porcupine.git", :tag => "Porcupine-iOS-v1.9.4"}
  s.ios.deployment_target = '9.0'
  s.swift_version = '5.0'
  s.vendored_frameworks = 'lib/ios/PvPorcupine.xcframework'
  s.resources = 'resources/keyword_files/ios/{alexa,americano,blueberry,bumblebee,computer,grapefruit,grasshopper,hey google,hey siri,jarvis,ok google,picovoice,porcupine,terminator}_ios.ppn', 
  'lib/common/porcupine_params.pv'
  s.source_files = 'binding/ios/*.{swift}'

  s.dependency 'ios-voice-processor', '~> 1.0.2'
end
