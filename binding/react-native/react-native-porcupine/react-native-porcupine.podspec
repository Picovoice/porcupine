require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "react-native-porcupine"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.description  = <<-DESC
                  react-native-porcupine
                   DESC
  s.homepage     = "https://github.com/ian/react-native-porcupine"
  # brief license entry:
  s.license      = "Apache-2.0"
  # optional - use expanded license entry instead:
  # s.license    = { :type => "Apache-2.0", :file => "LICENSE" }
  s.authors      = { "Ian Lavery" => "ian@picovoice.ai" }
  s.platforms    = { :ios => "9.0" }
  s.source       = { :git => "https://github.com/ian/react-native-porcupine.git", :tag => "#{s.version}" }

  s.source_files = "ios/**/*.{h,c,m,swift}"
  s.requires_arc = true

  s.dependency "React"
  # ...
  # s.dependency "..."
end

