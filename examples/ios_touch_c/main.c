#include <stdio.h>
#include <dlfcn.h>
#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>
 #include "RGFW.h"
 #include "rgfw_ios_entry_c.h"

static inline id nsstr(const char* s) {
    return ((id(*)(id,SEL,const char*))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), s);
}

static void attach_to_first_scene(void* uiWindow) {
    id app = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("UIApplication"), sel_registerName("sharedApplication"));
    id scenesSet = ((id(*)(id,SEL))objc_msgSend)(app, sel_registerName("connectedScenes"));
    id scenes = ((id(*)(id,SEL))objc_msgSend)(scenesSet, sel_registerName("allObjects"));
    unsigned long count = ((unsigned long(*)(id,SEL))objc_msgSend)(scenes, sel_registerName("count"));
    for (unsigned long i = 0; i < count; ++i) {
        id sc = ((id(*)(id,SEL,unsigned long))objc_msgSend)(scenes, sel_registerName("objectAtIndex:"), i);
        ((void(*)(id,SEL,id))objc_msgSend)((id)uiWindow, sel_registerName("setWindowScene:"), sc);
        ((void(*)(id,SEL))objc_msgSend)((id)uiWindow, sel_registerName("makeKeyAndVisible"));
        break;
    }
}

/* touch state */
static int t_active = 0;
static float t_x = 0, t_y = 0; /* points */

static void touch_cb(RGFW_window* w, RGFW_eventType type, const void* payload) {
    (void)w;
    typedef struct { RGFW_eventType type; RGFW_window* win; unsigned char count; struct { unsigned long long id; int x; int y; } touches[10]; } TouchView;
    const TouchView* tp = (const TouchView*)payload;
    const char* t = (type==RGFW_touchBegan?"began": type==RGFW_touchMoved?"moved": type==RGFW_touchEnded?"ended":"cancelled");
    if (tp && tp->count > 0) {
        t_active = (type == RGFW_touchEnded || type == RGFW_touchCancelled) ? 0 : 1;
        t_x = (float)tp->touches[0].x;
        t_y = (float)tp->touches[0].y;
        printf("touch %s: id=%llu at (%d,%d)\n", t, tp->touches[0].id, tp->touches[0].x, tp->touches[0].y);
    }
}

int app_main(void) {
    RGFW_setTouchCallback(touch_cb);
    RGFW_window* win = RGFW_createWindow("RGFW iOS Touch C-only", 0, 0, 0, 0, 0);
    void* uiWindow = RGFW_window_getWindow_iOS(win);
    if (uiWindow) attach_to_first_scene(uiWindow);

    /* setup Metal */
    id (*MTLCreateSystemDefaultDevice_fn)(void) = dlsym(RTLD_DEFAULT, "MTLCreateSystemDefaultDevice");
    id device = NULL, queue = NULL, layer = NULL, lib = NULL, vs = NULL, fs = NULL, desc = NULL, pipeline = NULL;
    if (MTLCreateSystemDefaultDevice_fn) {
        device = MTLCreateSystemDefaultDevice_fn();
        queue = ((id(*)(id,SEL))objc_msgSend)(device, sel_registerName("newCommandQueue"));

        void* uiView = RGFW_window_getView_iOS(win);
        layer = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("CAMetalLayer"), sel_registerName("layer"));
        ((void(*)(id,SEL,id))objc_msgSend)(layer, sel_registerName("setDevice:"), device);
        ((void(*)(id,SEL,unsigned long))objc_msgSend)(layer, sel_registerName("setPixelFormat:"), 80UL);
        CGRect vb = ((CGRect(*)(id,SEL))objc_msgSend)((id)uiView, sel_registerName("bounds"));
        ((void(*)(id,SEL,CGRect))objc_msgSend)(layer, sel_registerName("setFrame:"), vb);
        id viewLayer = ((id(*)(id,SEL))objc_msgSend)((id)uiView, sel_registerName("layer"));
        ((void(*)(id,SEL,id))objc_msgSend)(viewLayer, sel_registerName("addSublayer:"), layer);

        lib = ((id(*)(id,SEL))objc_msgSend)(device, sel_registerName("newDefaultLibrary"));
        if (!lib) {
            /* No metallib; continue as logger only */
            pipeline = NULL;
        } else {
            vs = ((id(*)(id,SEL,id))objc_msgSend)(lib, sel_registerName("newFunctionWithName:"), nsstr("v_main"));
            fs = ((id(*)(id,SEL,id))objc_msgSend)(lib, sel_registerName("newFunctionWithName:"), nsstr("f_main"));
            desc = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("MTLRenderPipelineDescriptor"), sel_registerName("new"));
            ((void(*)(id,SEL,id))objc_msgSend)(desc, sel_registerName("setVertexFunction:"), vs);
            ((void(*)(id,SEL,id))objc_msgSend)(desc, sel_registerName("setFragmentFunction:"), fs);
            id ca = ((id(*)(id,SEL))objc_msgSend)(desc, sel_registerName("colorAttachments"));
            id ca0 = ((id(*)(id,SEL,unsigned long))objc_msgSend)(ca, sel_registerName("objectAtIndexedSubscript:"), 0UL);
            ((void(*)(id,SEL,unsigned long))objc_msgSend)(ca0, sel_registerName("setPixelFormat:"), 80UL);
            pipeline = ((id(*)(id,SEL,id,void*))objc_msgSend)(device, sel_registerName("newRenderPipelineStateWithDescriptor:error:"), desc, NULL);
        }
    }

    /* main loop */
    while (!RGFW_window_shouldClose(win)) {
        RGFW_event e; while (RGFW_window_checkEvent(win, &e)) { /* input handled by callback */ }

        if (pipeline) {
            id drawable = ((id(*)(id,SEL))objc_msgSend)(layer, sel_registerName("nextDrawable"));
            if (drawable) {
                id tex = ((id(*)(id,SEL))objc_msgSend)(drawable, sel_registerName("texture"));
                id rp = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("MTLRenderPassDescriptor"), sel_registerName("renderPassDescriptor"));
                id carr = ((id(*)(id,SEL))objc_msgSend)(rp, sel_registerName("colorAttachments"));
                id att0 = ((id(*)(id,SEL,unsigned long))objc_msgSend)(carr, sel_registerName("objectAtIndexedSubscript:"), 0UL);
                ((void(*)(id,SEL,id))objc_msgSend)(att0, sel_registerName("setTexture:"), tex);
                ((void(*)(id,SEL,unsigned long))objc_msgSend)(att0, sel_registerName("setLoadAction:"), 2UL /* Clear */);
                ((void(*)(id,SEL,unsigned long))objc_msgSend)(att0, sel_registerName("setStoreAction:"), 1UL /* Store */);

                id cb = ((id(*)(id,SEL))objc_msgSend)(queue, sel_registerName("commandBuffer"));
                id enc = ((id(*)(id,SEL,id))objc_msgSend)(cb, sel_registerName("renderCommandEncoderWithDescriptor:"), rp);
                ((void(*)(id,SEL,id))objc_msgSend)(enc, sel_registerName("setRenderPipelineState:"), pipeline);

                if (t_active) {
                    /* Draw a small square at t_x,t_y */
                    /* Convert to NDC using view bounds */
                    void* uiView = RGFW_window_getView_iOS(win);
                    CGRect vb = ((CGRect(*)(id,SEL))objc_msgSend)((id)uiView, sel_registerName("bounds"));
                    float vw = (float)vb.size.width, vh = (float)vb.size.height;
                    float half = 12.0f;
                    float x0 = (t_x - half) / vw * 2.0f - 1.0f;
                    float y0 = 1.0f - (t_y - half) / vh * 2.0f;
                    float x1 = (t_x + half) / vw * 2.0f - 1.0f;
                    float y1 = 1.0f - (t_y + half) / vh * 2.0f;
                    float verts[8] = { x0,y0, x1,y0, x1,y1, x0,y1 };
                    float color[4] = { 1.0f, 0.2f, 0.2f, 1.0f };
                    /* set buffers via setVertexBytes:... indices 0 and 1 (matches Shaders.metal) */
                    ((void(*)(id,SEL,const void*,unsigned long,unsigned long))objc_msgSend)(enc, sel_registerName("setVertexBytes:length:atIndex:"), verts, sizeof(verts), 0UL);
                    ((void(*)(id,SEL,const void*,unsigned long,unsigned long))objc_msgSend)(enc, sel_registerName("setVertexBytes:length:atIndex:"), color, sizeof(color), 1UL);
                    /* Draw two triangles (indices on the fly) */
                    uint16_t idx[6] = {0,1,2, 2,3,0};
                    id ib = ((id(*)(id,SEL,const void*,unsigned long,unsigned long))objc_msgSend)(device, sel_registerName("newBufferWithBytes:length:options:"), idx, sizeof(idx), 0UL);
                    ((void(*)(id,SEL,unsigned long,unsigned long,unsigned long,id,unsigned long))objc_msgSend)(enc, sel_registerName("drawIndexedPrimitives:indexCount:indexType:indexBuffer:indexBufferOffset:"), 3UL, 6UL, 0UL /* UInt16 */, ib, 0UL);
                }
                ((void(*)(id,SEL))objc_msgSend)(enc, sel_registerName("endEncoding"));
                ((void(*)(id,SEL,id))objc_msgSend)(cb, sel_registerName("presentDrawable:"), drawable);
                ((void(*)(id,SEL))objc_msgSend)(cb, sel_registerName("commit"));
            }
        }
    }
    RGFW_window_close(win);
    return 0;
}

RGFW_IOS_C_APP(app_main)
