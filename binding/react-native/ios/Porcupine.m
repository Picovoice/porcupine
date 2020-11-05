#import <React/RCTBridgeModule.h>

@interface RCT_EXTERN_MODULE(RCTPorcupine, NSObject)

RCT_EXTERN_METHOD(create: (NSString *)modelPath keywordPath: (NSString *)keywordPath
                  sensitivity: (float)sensitivity)

RCT_EXTERN_METHOD(delete)

RCT_EXTERN_METHOD(process: (NSArray<NSNumber>)pcm callback: (RCTResponseSenderBlock)callback)

RCT_EXTERN_METHOD(getFrameLength: (RCTResponseSenderBlock)callback)

RCT_EXTERN_METHOD(getSampleRate: (RCTResponseSenderBlock)callback)

RCT_EXTERN_METHOD(getVersion: (RCTResponseSenderBlock)callback)


@end
