//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#import <Foundation/Foundation.h>

@interface OpenGLWrapper : NSObject

- (void)Init;
- (void)Render;
- (void)Clear;
- (void)UpdateGlViewPort;

@end
