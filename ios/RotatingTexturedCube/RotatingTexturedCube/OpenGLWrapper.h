//
//  OpenGLWrapper.h
//  RotatingTexturedCube
//
//  Created by Marvin Gajek on 23.03.24.
//

#import <Foundation/Foundation.h>

@interface OpenGLWrapper : NSObject

- (void)Init;
- (void)Render;
- (void)Clear;
- (void)UpdateGlViewPort: (int)width height:(int)height;

@end
