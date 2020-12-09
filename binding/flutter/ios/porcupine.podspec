Pod::Spec.new do |s|
  s.name             = 'porcupine'
  s.version          = '1.9.0'
  s.summary          = 'A Flutter package plugin for Picovoice\'s Porcupine wake word engine'
  s.description      = <<-DESC
  A Flutter package plugin for Picovoice\'s Porcupine wake word engine.
                       DESC
  s.homepage         = 'https://picovoice.ai/'
  s.license          = { :type => 'Apache-2.0' }
  s.author           = { 'Picovoice' => 'hello@picovoice.ai' }
  s.source           = { :git => "https://github.com/Picovoice/porcupine.git" }
  s.source_files = 'Classes/**/*'
  s.platform = :ios, '8.0'
  s.dependency 'Flutter'
  s.static_framework = true

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 
    'OTHER_LDFLAGS' => '-ObjC', 
    'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386'}

    s.subspec 'pv_porcupine' do |sc|    
      sc.pod_target_xcconfig = {
        'DEAD_CODE_STRIPPING' => 'NO',
        'SWIFT_INCLUDE_PATHS' => '$(PODS_TARGET_SRCROOT)/pv_porcupine',
        'OTHER_CFLAGS' => '-Xcc -fmodule-map-file="${PODS_TARGET_SRCROOT}/pv_porcupine/module.private.modulemap"',
        'OTHER_SWIFT_FLAGS' => '-Xcc -fmodule-map-file="${PODS_TARGET_SRCROOT}/pv_porcupine/module.private.modulemap"'
      }
      
      sc.vendored_libraries = 'pv_porcupine/libpv_porcupine.a'
      sc.source_files = 'pv_porcupine/pv_porcupine.h', 'pv_porcupine/picovoice.h'
      sc.preserve_paths = 'pv_porcupine/libpv_porcupine.a', 'pv_porcupine/module.private.modulemap'
    end

  s.swift_version = '5.0'
end