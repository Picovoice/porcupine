#import <React/RCTBridgeModule.h>

@interface RCT_EXTERN_MODULE(PvPorcupine, NSObject)

RCT_EXTERN_METHOD(create: (NSString *)modelPath 
                  keywordPaths: (NSArray<NSString *>)keywordPaths 
                  sensitivities: (NSArray<NSNumber>)sensitivities 
                  resolver: (RCTPromiseResolveBlock)resolve 
                  rejecter: (RCTPromiseRejectBlock)reject)

RCT_EXTERN_METHOD(delete: (NSString *)handle)

RCT_EXTERN_METHOD(process: (NSString *)handle 
                  pcm:(NSArray<NSNumber>)pcm 
                  resolver: (RCTPromiseResolveBlock)resolve 
                  rejecter: (RCTPromiseRejectBlock)reject)

@end
