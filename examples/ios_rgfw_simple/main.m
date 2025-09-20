#import <UIKit/UIKit.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import "RGFW.h"
#import "rgfw_ios_entry.h"

static id<MTLDevice> g_device;
static id<MTLCommandQueue> g_queue;
static id<MTLLibrary> g_lib;
static id<MTLRenderPipelineState> g_pipeline;
static CAMetalLayer* g_layer;

static void setup_metal(UIView* view) {
    g_device = MTLCreateSystemDefaultDevice();
    g_queue = [g_device newCommandQueue];

    g_layer = [CAMetalLayer layer];
    g_layer.device = g_device;
    g_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    g_layer.framebufferOnly = YES;
    g_layer.contentsScale = UIScreen.mainScreen.scale;
    g_layer.frame = view.bounds;
    [view.layer addSublayer:g_layer];

    NSError* err = nil;
    g_lib = [g_device newDefaultLibrary];
    if (!g_lib) {
        // Fallback: compile inline library
        id<MTLLibrary> lib = [g_device newLibraryWithSource:
            @"#include <metal_stdlib>\n"
             "using namespace metal;\n"
             "struct VSOut { float4 pos [[position]]; float4 col; };\n"
             "vertex VSOut v_main(uint vid [[vertex_id]]) {\n"
             "  float2 p[3] = { float2(0.0, 0.8), float2(-0.8,-0.8), float2(0.8,-0.8) };\n"
             "  float4 c[3] = { float4(1,0,0,1), float4(0,1,0,1), float4(0,0,1,1) };\n"
             "  VSOut o; o.pos=float4(p[vid],0,1); o.col=c[vid]; return o; }\n"
             "fragment float4 f_main(VSOut in [[stage_in]]) { return in.col; }\n"
        options:nil error:&err];
        g_lib = lib;
    }

    id<MTLFunction> vs = [g_lib newFunctionWithName:@"v_main"];
    id<MTLFunction> fs = [g_lib newFunctionWithName:@"f_main"];

    MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
    desc.vertexFunction = vs;
    desc.fragmentFunction = fs;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    g_pipeline = [g_device newRenderPipelineStateWithDescriptor:desc error:&err];
}

static void draw_frame(void) {
    @autoreleasepool {
        g_layer.frame = g_layer.superlayer.bounds;
        id<CAMetalDrawable> drawable = [g_layer nextDrawable];
        if (!drawable) return;
        MTLRenderPassDescriptor* rp = [MTLRenderPassDescriptor renderPassDescriptor];
        rp.colorAttachments[0].texture = drawable.texture;
        rp.colorAttachments[0].loadAction = MTLLoadActionClear;
        rp.colorAttachments[0].storeAction = MTLStoreActionStore;
        rp.colorAttachments[0].clearColor = MTLClearColorMake(0.1, 0.1, 0.12, 1.0);
        id<MTLCommandBuffer> cb = [g_queue commandBuffer];
        id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:rp];
        [enc setRenderPipelineState:g_pipeline];
        [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
        [enc endEncoding];
        [cb presentDrawable:drawable];
        [cb commit];
    }
}

int app_main(void) {
    RGFW_window* win = RGFW_createWindow("RGFW iOS (Simple)", 0, 0, 0, 0, 0);
    // Attach RGFW UIWindow to the first available UIWindowScene
    UIWindow* uiWindow = (__bridge UIWindow*)RGFW_window_getWindow_iOS(win);
    for (UIScene* sc in UIApplication.sharedApplication.connectedScenes) {
        if ([sc isKindOfClass:[UIWindowScene class]]) {
            [uiWindow setWindowScene:(UIWindowScene*)sc];
            break;
        }
    }
    [uiWindow makeKeyAndVisible];
    UIView* view = (__bridge UIView*)RGFW_window_getView_iOS(win);
    setup_metal(view);

    while (!RGFW_window_shouldClose(win)) {
        RGFW_event e; while (RGFW_window_checkEvent(win, &e)) { /* handle as needed */ }
        draw_frame();
    }
    RGFW_window_close(win);
    return 0;
}

RGFW_IOS_APP(app_main);
