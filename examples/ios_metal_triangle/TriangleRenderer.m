#import "TriangleRenderer.h"
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>

@interface TriangleRenderer ()
@property (nonatomic, weak) UIView* view;
@property (nonatomic, strong) CAMetalLayer* metalLayer;
@property (nonatomic, strong) id<MTLDevice> device;
@property (nonatomic, strong) id<MTLCommandQueue> queue;
@property (nonatomic, strong) id<MTLRenderPipelineState> pipeline;
@end

@implementation TriangleRenderer

- (instancetype)initWithView:(UIView*)view {
    if ((self = [super init])) {
        _view = view;
        _device = MTLCreateSystemDefaultDevice();
        _queue = [_device newCommandQueue];

        _metalLayer = [CAMetalLayer layer];
        _metalLayer.device = _device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        _metalLayer.framebufferOnly = YES;
        _metalLayer.contentsScale = UIScreen.mainScreen.scale;
        _metalLayer.frame = view.bounds;
        [view.layer addSublayer:_metalLayer];

        NSError* err = nil;
        id<MTLLibrary> lib = [_device newDefaultLibrary];
        if (!lib) {
            NSLog(@"Metal default library not found. Ensure Shaders.metal is in the target.");
            return self;
        }
        id<MTLFunction> vfn = [lib newFunctionWithName:@"v_main"];
        id<MTLFunction> ffn = [lib newFunctionWithName:@"f_main"];
        MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
        desc.vertexFunction = vfn;
        desc.fragmentFunction = ffn;
        desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        _pipeline = [_device newRenderPipelineStateWithDescriptor:desc error:&err];
        if (!_pipeline) {
            NSLog(@"Metal pipeline error: %@", err);
        }
    }
    return self;
}

- (void)draw {
    if (!self.pipeline) return;
    @autoreleasepool {
        self.metalLayer.frame = self.view.bounds;
        id<CAMetalDrawable> drawable = [self.metalLayer nextDrawable];
        if (!drawable) return;
        MTLRenderPassDescriptor* rp = [MTLRenderPassDescriptor renderPassDescriptor];
        rp.colorAttachments[0].texture = drawable.texture;
        rp.colorAttachments[0].loadAction = MTLLoadActionClear;
        rp.colorAttachments[0].storeAction = MTLStoreActionStore;
        rp.colorAttachments[0].clearColor = MTLClearColorMake(0.1, 0.1, 0.12, 1.0);
        id<MTLCommandBuffer> cb = [self.queue commandBuffer];
        id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:rp];
        [enc setRenderPipelineState:self.pipeline];
        [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
        [enc endEncoding];
        [cb presentDrawable:drawable];
        [cb commit];
    }
}

@end
