Pod::Spec.new do |s|
  s.name             = 'porcupine_flutter'
  s.version          = '2.2.0'
  s.summary          = 'A Flutter package plugin for Picovoice\'s Porcupine wake word engine'
  s.description      = <<-DESC
  A Flutter package plugin for Picovoice\'s Porcupine wake word engine.
                       DESC
  s.homepage         = 'https://picovoice.ai/'
  s.license          = { :type => 'Apache-2.0' }
  s.author           = { 'Picovoice' => 'hello@picovoice.ai' }
  s.source           = { :git => "https://github.com/Picovoice/porcupine.git" }
  s.source_files = 'Classes/**/*'
  s.platform = :ios, '11.0'
  s.dependency 'Flutter'
  s.dependency 'Porcupine-iOS', '~> 2.2.0'

  s.swift_version = '5.0'
end