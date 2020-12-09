#import "PorcupinePlugin.h"
#if __has_include(<porcupine/porcupine-Swift.h>)
#import <porcupine/porcupine-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "porcupine-Swift.h"
#endif

@implementation PorcupinePlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftPorcupinePlugin registerWithRegistrar:registrar];  
}
@end
