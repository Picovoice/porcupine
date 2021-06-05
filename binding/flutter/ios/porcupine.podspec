Pod::Spec.new do |s|
  s.name             = 'porcupine'
  s.version          = '1.9.12'
  s.summary          = 'A Flutter package plugin for Picovoice\'s Porcupine wake word engine'
  s.description      = <<-DESC
  A Flutter package plugin for Picovoice\'s Porcupine wake word engine.
                       DESC
  s.homepage         = 'https://picovoice.ai/'
  s.license          = { :type => 'Apache-2.0' }
  s.author           = { 'Picovoice' => 'hello@picovoice.ai' }
  s.source           = { :git => "https://github.com/Picovoice/porcupine.git" }
  s.source_files = 'Classes/**/*'
  s.platform = :ios, '9.0'
  s.dependency 'Flutter'
  s.vendored_frameworks = "PvPorcupine.xcframework"

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = 
  {
    'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386'
  }

  s.swift_version = '5.0'
end