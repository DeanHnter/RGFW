#import <UIKit/UIKit.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <simd/simd.h>
#import "RGFW.h"
#import "rgfw_ios_entry.h"

static id<MTLDevice> t_device;
static id<MTLCommandQueue> t_queue;
static id<MTLLibrary> t_lib;
static id<MTLRenderPipelineState> t_pipeline;
static CAMetalLayer* t_layer;
static float t_viewW = 0.0f, t_viewH = 0.0f;

static BOOL t_touchActive = NO;
static float t_touchX = 0.0f, t_touchY = 0.0f; /* in points */

typedef struct { RGFW_eventType type; RGFW_window* win; uint8_t count; struct { uint64_t id; int32_t x; int32_t y; } touches[10]; } RGFW_TouchPayloadView;
static void touch_cb(RGFW_window* w, RGFW_eventType type, const void* payload) {
    (void)w;
    const RGFW_TouchPayloadView* tp = (const RGFW_TouchPayloadView*)payload;
    if (type == RGFW_touchBegan || type == RGFW_touchMoved) {
        if (tp->count > 0) {
            t_touchActive = YES;
            t_touchX = (float)tp->touches[0].x;
            t_touchY = (float)tp->touches[0].y;
        }
    } else if (type == RGFW_touchEnded || type == RGFW_touchCancelled) {
        t_touchActive = NO;
    }
}

static void setup_metal(UIView* view) {
    t_device = MTLCreateSystemDefaultDevice();
    t_queue = [t_device newCommandQueue];

    t_layer = [CAMetalLayer layer];
    t_layer.device = t_device;
    t_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    t_layer.framebufferOnly = YES;
    t_layer.contentsScale = UIScreen.mainScreen.scale;
    t_layer.frame = view.bounds;
    [view.layer addSublayer:t_layer];
    t_viewW = (float)view.bounds.size.width;
    t_viewH = (float)view.bounds.size.height;

    NSError* err = nil;
    t_lib = [t_device newDefaultLibrary];
    if (!t_lib) {
        NSLog(@"Metal default library not found. Ensure Shaders.metal is included in the target.");
        return;
    }

    id<MTLFunction> vs = [t_lib newFunctionWithName:@"v_main"];
    id<MTLFunction> fs = [t_lib newFunctionWithName:@"f_main"];
    MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
    desc.vertexFunction = vs;
    desc.fragmentFunction = fs;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    t_pipeline = [t_device newRenderPipelineStateWithDescriptor:desc error:&err];
}

static inline void to_ndc(float x_pt, float y_pt, float* x_ndc, float* y_ndc) {
    *x_ndc = (x_pt / t_viewW) * 2.0f - 1.0f;
    *y_ndc = 1.0f - (y_pt / t_viewH) * 2.0f;
}

static void draw_frame(void) {
    @autoreleasepool {
        t_layer.frame = t_layer.superlayer.bounds;
        t_viewW = (float)t_layer.bounds.size.width;
        t_viewH = (float)t_layer.bounds.size.height;
        id<CAMetalDrawable> drawable = [t_layer nextDrawable];
        if (!drawable) return;
        MTLRenderPassDescriptor* rp = [MTLRenderPassDescriptor renderPassDescriptor];
        rp.colorAttachments[0].texture = drawable.texture;
        rp.colorAttachments[0].loadAction = MTLLoadActionClear;
        rp.colorAttachments[0].storeAction = MTLStoreActionStore;
        rp.colorAttachments[0].clearColor = MTLClearColorMake(0.08, 0.08, 0.1, 1.0);
        id<MTLCommandBuffer> cb = [t_queue commandBuffer];
        id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:rp];
        [enc setRenderPipelineState:t_pipeline];

        if (t_touchActive) {
            float x = t_touchX, y = t_touchY;
            float half = 10.0f; // 20x20 pt square
            float nx0, ny0, nx1, ny1, nx2, ny2, nx3, ny3;
            to_ndc(x - half, y - half, &nx0, &ny0);
            to_ndc(x + half, y - half, &nx1, &ny1);
            to_ndc(x + half, y + half, &nx2, &ny2);
            to_ndc(x - half, y + half, &nx3, &ny3);
            float verts[8] = { nx0, ny0, nx1, ny1, nx2, ny2, nx3, ny3 };
            simd_float4 color = (simd_float4){1.0, 0.2, 0.2, 1.0};
            [enc setVertexBytes:verts length:sizeof(verts) atIndex:0];
            [enc setVertexBytes:&color length:sizeof(color) atIndex:1];
            // two triangles: 0-1-2 and 2-3-0
            uint16_t idx[6] = {0,1,2, 2,3,0};
            [enc drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:6 indexType:MTLIndexTypeUInt16 indexBuffer:[t_device newBufferWithBytes:idx length:sizeof(idx) options:MTLResourceStorageModeShared] indexBufferOffset:0];
        } else {
            // optional: draw nothing when inactive
        }

        [enc endEncoding];
        [cb presentDrawable:drawable];
        [cb commit];
    }
}

int app_main(void) {
    RGFW_setTouchCallback(touch_cb);
    RGFW_window* win = RGFW_createWindow("RGFW iOS Touch", 0, 0, 0, 0, 0);
    // attach UIWindow to a UIWindowScene to ensure visibility
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
