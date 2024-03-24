//
//  OpenGLWrapper.m
//  RotatingTexturedCube
//
//  Created by Marvin Gajek on 23.03.24.
//

#import "OpenGLWrapper.h"
#import "OpenGLRenderer.hpp"

@implementation OpenGLWrapper {
    OpenGLRenderer *renderer;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        renderer = new OpenGLRenderer();
    }
    return self;
}

- (void)dealloc {
    delete renderer;
}

- (void)updateGlViewPortWithWidth:(int)width height:(int)height {
    [renderer updateGlViewPortWithWidth:width height:height];
}

- (void)render {
    [renderer render];
}

@end

