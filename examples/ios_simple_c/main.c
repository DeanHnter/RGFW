#include <stdio.h>
#include <dlfcn.h>
#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>
#include "RGFW.h" /* RGFW_IOS_APP macro is inside RGFW.h now */

int app_main(void) {
    RGFW_window* win = RGFW_createWindow("RGFW iOS C-only", 0, 0, 0, 0, 0);
    RGFW_attachWindowToFirstScene_iOS(win);

    /* Color the content view so the app isn't just black */
    void* uiView = RGFW_window_getView_iOS(win);
    if (uiView) {
        id uicolor = ((id(*)(id,SEL,double,double,double,double))objc_msgSend)
            ((id)objc_getClass("UIColor"), sel_registerName("colorWithRed:green:blue:alpha:"), 0.20, 0.60, 0.90, 1.0);
        ((void(*)(id,SEL,id))objc_msgSend)
            ((id)uiView, sel_registerName("setBackgroundColor:"), uicolor);
    }

    /* Minimal Metal clear+triangle via runtime */
    id (*MTLCreateSystemDefaultDevice_fn)(void) = dlsym(RTLD_DEFAULT, "MTLCreateSystemDefaultDevice");
    id device = NULL, queue = NULL, layer = NULL, lib = NULL, vs = NULL, fs = NULL, desc = NULL, pipeline = NULL;
    if (MTLCreateSystemDefaultDevice_fn) {
        device = MTLCreateSystemDefaultDevice_fn();
        queue = ((id(*)(id,SEL))objc_msgSend)(device, sel_registerName("newCommandQueue"));

        layer = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("CAMetalLayer"), sel_registerName("layer"));
        ((void(*)(id,SEL,id))objc_msgSend)(layer, sel_registerName("setDevice:"), device);
        ((void(*)(id,SEL,unsigned long))objc_msgSend)(layer, sel_registerName("setPixelFormat:"), 80 /* MTLPixelFormatBGRA8Unorm */);
        /* set frame = view.bounds */
        CGRect vb = ((CGRect(*)(id,SEL))objc_msgSend)((id)uiView, sel_registerName("bounds"));
        ((void(*)(id,SEL,CGRect))objc_msgSend)(layer, sel_registerName("setFrame:"), vb);
        id viewLayer = ((id(*)(id,SEL))objc_msgSend)((id)uiView, sel_registerName("layer"));
        ((void(*)(id,SEL,id))objc_msgSend)(viewLayer, sel_registerName("addSublayer:"), layer);

        /* Load default metallib compiled from Shaders.metal */
        lib = ((id(*)(id,SEL))objc_msgSend)(device, sel_registerName("newDefaultLibrary"));
        if (!lib) goto skip_draw;
        id vsName = ((id(*)(id,SEL,const char*))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "v_main");
        id fsName = ((id(*)(id,SEL,const char*))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "f_main");
        vs = ((id(*)(id,SEL,id))objc_msgSend)(lib, sel_registerName("newFunctionWithName:"), vsName);
        fs = ((id(*)(id,SEL,id))objc_msgSend)(lib, sel_registerName("newFunctionWithName:"), fsName);
        desc = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("MTLRenderPipelineDescriptor"), sel_registerName("new"));
        ((void(*)(id,SEL,id))objc_msgSend)(desc, sel_registerName("setVertexFunction:"), vs);
        ((void(*)(id,SEL,id))objc_msgSend)(desc, sel_registerName("setFragmentFunction:"), fs);
        id ca = ((id(*)(id,SEL))objc_msgSend)(desc, sel_registerName("colorAttachments"));
        id ca0 = ((id(*)(id,SEL,unsigned long))objc_msgSend)(ca, sel_registerName("objectAtIndexedSubscript:"), 0UL);
        ((void(*)(id,SEL,unsigned long))objc_msgSend)(ca0, sel_registerName("setPixelFormat:"), 80UL);
        pipeline = ((id(*)(id,SEL,id,void*))objc_msgSend)(device, sel_registerName("newRenderPipelineStateWithDescriptor:error:"), desc, NULL);

        /* One draw to avoid pure black */
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
            ((void(*)(id,SEL,unsigned long,unsigned long,unsigned long))objc_msgSend)(enc, sel_registerName("drawPrimitives:vertexStart:vertexCount:"), 3UL /* triangle */, 0UL, 3UL);
            ((void(*)(id,SEL))objc_msgSend)(enc, sel_registerName("endEncoding"));
            ((void(*)(id,SEL,id))objc_msgSend)(cb, sel_registerName("presentDrawable:"), drawable);
            ((void(*)(id,SEL))objc_msgSend)(cb, sel_registerName("commit"));
        }
skip_draw:
    }

    while (!RGFW_window_shouldClose(win)) {
        RGFW_event e; while (RGFW_window_checkEvent(win, &e)) {
            if (e.type == RGFW_quit) break;
        }
    }
    RGFW_window_close(win);
    return 0;
}

RGFW_IOS_APP(app_main)
