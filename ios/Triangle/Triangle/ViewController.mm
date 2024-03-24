//
//  ViewController.m
//  Triangle
//
//  Created by Marvin Gajek on 23.03.24.
//

#import "ViewController.h"
#include "Triangle.h"
#import <GLKit/GLKit.h>

@interface ViewController () <GLKViewDelegate> // Adopt GLKViewDelegate protocol
{
    Triangle triangle;
}
@property (nonatomic, strong) EAGLContext *context;
@property (nonatomic, strong) GLKView *glkView;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Initialize OpenGL
    [self setupOpenGL];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    // Cleanup OpenGL
    [self cleanupOpenGL];
}

- (void)setupOpenGL {
    // Create an OpenGL ES 3.0 context
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
        return;
    }
    
    // Set the context to the current context
    [EAGLContext setCurrentContext:self.context];
    
    // Create a GLKView with the same size as the current view
    self.glkView = [[GLKView alloc] initWithFrame:self.view.bounds context:self.context];
    self.glkView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    self.glkView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    self.glkView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    self.glkView.drawableMultisample = GLKViewDrawableMultisample4X;
    self.glkView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    
    // Set the delegate to self
    self.glkView.delegate = self;
    
    // Add the GLKView to the view hierarchy
    [self.view addSubview:self.glkView];
    
    // Initialize OpenGL content
    triangle.initOpenGL();
}

- (void)cleanupOpenGL {
    // Cleanup OpenGL
    triangle.clearOpenGL();
    
    // Clear the context
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    // Draw OpenGL content
    triangle.drawOpenGL();
}

@end
