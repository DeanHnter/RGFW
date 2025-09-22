
	#define RGFW_EXPORT - use when building RGFW
	#define RGFW_IMPORT - use when linking with RGFW (not as a single-header)

	#define RGFW_USE_INT - force the use c-types rather than stdint.h (for systems that might not have stdint.h (msvc))
	#define RGFW_bool x - choose what type to use for bool, by default u32 is used
*/

/*
Example to get you started :

linux : gcc main.c -lX11 -lXrandr -lGL
/* plus it helps with cross-compiling */

#ifdef __EMSCRIPTEN__
	#define RGFW_WASM
#endif

#if defined(__APPLE__)
    #include <TargetConditionals.h>
#endif

#if defined(RGFW_X11) && defined(__APPLE__) && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_MACOS_X11
	#define RGFW_UNIX
#endif

#if defined(_WIN32) && !defined(RGFW_X11) && !defined(RGFW_UNIX) && !defined(RGFW_WASM) && !defined(RGFW_CUSTOM_BACKEND) /* (if you're using X11 on windows some how) */
		#define RGFW_EGL
	#endif
	#ifdef RGFW_X11
		#define RGFW_DYNAMIC
	#endif
#endif
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_IOS
	#define RGFW_APPLE
#elif (!defined(RGFW_WAYLAND) && !defined(RGFW_X11)) && (defined(__unix__) || defined(RGFW_MACOS_X11) || defined(RGFW_X11))  && !defined(RGFW_WASM)  && !defined(RGFW_CUSTOM_BACKEND)
		#define RGFW_MACOS_X11
		#define RGFW_X11
		#define RGFW_UNIX
#elif defined(__APPLE__) && !defined(RGFW_MACOS_X11) && !defined(RGFW_X11)  && !defined(RGFW_WASM)  && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_MACOS
#endif
		This is also the size of the array which stores all the dropped file string,
		RGFW_event.drop.files
	*/
	RGFW_windowMaximized, /*!< the window was maximized */
	RGFW_windowMinimized, /*!< the window was minimized */
	RGFW_windowRestored, /*!< the window was restored */
	RGFW_scaleUpdated, /*!< content scale factor changed */
	/* iOS touch events */
	RGFW_touchBegan,
	RGFW_touchMoved,
	RGFW_touchEnded,
	RGFW_touchCancelled
};


typedef RGFW_ENUM(u32, RGFW_eventFlag) {
    RGFW_keyPressedFlag = RGFW_BIT(RGFW_keyPressed),
    RGFW_keyReleasedFlag = RGFW_BIT(RGFW_keyReleased),
    RGFW_windowRestoredFlag = RGFW_BIT(RGFW_windowRestored),
    RGFW_scaleUpdatedFlag = RGFW_BIT(RGFW_scaleUpdated),
    RGFW_quitFlag = RGFW_BIT(RGFW_quit),
    RGFW_dataDropFlag = RGFW_BIT(RGFW_dataDrop),
    RGFW_dataDragFlag = RGFW_BIT(RGFW_dataDrag),
    RGFW_touchBeganFlag = RGFW_BIT(RGFW_touchBegan),
    RGFW_touchMovedFlag = RGFW_BIT(RGFW_touchMoved),
    RGFW_touchEndedFlag = RGFW_BIT(RGFW_touchEnded),
    RGFW_touchCancelledFlag = RGFW_BIT(RGFW_touchCancelled),
    RGFW_keyEventsFlag = RGFW_keyPressedFlag | RGFW_keyReleasedFlag,
    RGFW_mouseEventsFlag = RGFW_mouseButtonPressedFlag | RGFW_mouseButtonReleasedFlag | RGFW_mousePosChangedFlag | RGFW_mouseEnterFlag | RGFW_mouseLeaveFlag | RGFW_mouseScrollFlag ,
    RGFW_windowEventsFlag = RGFW_windowMovedFlag | RGFW_windowResizedFlag | RGFW_windowRefreshFlag | RGFW_windowMaximizedFlag | RGFW_windowMinimizedFlag | RGFW_windowRestoredFlag | RGFW_scaleUpdatedFlag,
    RGFW_focusEventsFlag = RGFW_focusInFlag | RGFW_focusOutFlag,
    RGFW_dataDropEventsFlag = RGFW_dataDropFlag | RGFW_dataDragFlag,
    RGFW_touchEventsFlag = RGFW_touchBeganFlag | RGFW_touchMovedFlag | RGFW_touchEndedFlag | RGFW_touchCancelledFlag,
    RGFW_allEventFlags = RGFW_keyEventsFlag | RGFW_mouseEventsFlag | RGFW_windowEventsFlag | RGFW_focusEventsFlag | RGFW_dataDropEventsFlag | RGFW_touchEventsFlag | RGFW_quitFlag
};

/*! Event structure(s) and union for checking/getting events */
typedef struct RGFW_commonEvent {
	RGFW_eventType type; /*!< which event has been sent?*/
	RGFW_window* win; /*!< the window this event applies too (for event queue events) */
	RGFW_mouseScrollEvent scroll; /*!< data for a mouse scroll */
	RGFW_mousePosEvent mouse; /*!< data for mouse motion events */
	RGFW_keyEvent key; /*!< data for key press/release/hold events */
	RGFW_dataDropEvent drop; /*!< dropping a file events */
	RGFW_dataDragEvent drag; /* data for dragging a file events */
	RGFW_scaleUpdatedEvent scale; /* data for monitor scaling events */
	struct { /* touch payload */
		RGFW_eventType type; RGFW_window* win; u8 count; struct { u64 id; i32 x; i32 y; } touches[10];
	} touch;
} RGFW_event;

/*!
	for RGFW_the code is stupid and C++ waitForEvent
	waitMS -> Allows the function to keep checking for events even after there are no more events
			  if waitMS == 0, the loop will not wait for events
RGFWDEF void* RGFW_window_getView_OSX(RGFW_window* win);
RGFWDEF void* RGFW_window_getWindow_OSX(RGFW_window* win);
RGFWDEF void* RGFW_window_getHWND(RGFW_window* win);
RGFWDEF void* RGFW_window_getHDC(RGFW_window* win);
RGFWDEF u64 RGFW_window_getWindow_X11(RGFW_window* win);
RGFWDEF struct wl_surface* RGFW_window_getWindow_Wayland(RGFW_window* win);
RGFWDEF void* RGFW_window_getWindow_iOS(RGFW_window* win);
RGFWDEF void* RGFW_window_getView_iOS(RGFW_window* win);
RGFWDEF void* RGFW_window_getViewController_iOS(RGFW_window* win);
RGFWDEF void* RGFW_window_getWindowScene_iOS(RGFW_window* win);

/** * @defgroup Window_management
* @{ */

/*! set the window flags (will undo flags if they don't match the old ones) */
RGFWDEF void RGFW_window_setFlags(RGFW_window* win, RGFW_windowFlags);
	RGFW_warningWayland, RGFW_warningOpenGL
};

typedef void (* RGFW_debugfunc)(RGFW_debugType type, RGFW_errorCode err, const char* msg);
RGFWDEF RGFW_debugfunc RGFW_setDebugCallback(RGFW_debugfunc func);
RGFWDEF void RGFW_sendDebugInfo(RGFW_debugType type, RGFW_errorCode err, const char* msg);

/** iOS simple entry (C-only) */
#ifdef RGFW_IOS
typedef int (*RGFW_iOSEntryFn)(void);
RGFWDEF int RGFW_iOS_run(int argc, char** argv, RGFW_iOSEntryFn userMain);
RGFWDEF void RGFW_attachWindowToFirstScene_iOS(struct RGFW_window* win);
#define RGFW_IOS_APP(user_main_fn) int main(int argc, char** argv){ return RGFW_iOS_run(argc, argv, user_main_fn); }
#endif

/** @} */

/**


	event callbacks.
/*! RGFW_mouseScroll, the window that got the event, the x scroll value, the y scroll value */
typedef void (* RGFW_mouseScrollfunc)(RGFW_window* win, float x, float y);
/*! RGFW_dataDrop the window that had the drop, the drop data and the number of files dropped */
typedef void (* RGFW_dataDropfunc)(RGFW_window* win, char** files, size_t count);
/*! RGFW_scaleUpdated, the window the event was sent to, content scaleX, content scaleY */
typedef void (* RGFW_scaleUpdatedfunc)(RGFW_window* win, float scaleX, float scaleY);
/*! RGFW_touch* events (iOS), payload points to RGFW_event.touch */
typedef void (* RGFW_touchfunc)(RGFW_window* win, RGFW_eventType type, const void* payload);

/*! set callback for a window move event. Returns previous callback function (if it was set)  */
RGFWDEF RGFW_windowMovedfunc RGFW_setWindowMovedCallback(RGFW_windowMovedfunc func);
/*! set callback for a window resize event. Returns previous callback function (if it was set)  */
RGFWDEF RGFW_windowResizedfunc RGFW_setWindowResizedCallback(RGFW_windowResizedfunc func);
/*! set callback for a window quit event. Returns previous callback function (if it was set)  */
/*! set call back for when window is minimized. Returns the previous callback function (if it was set) */
RGFWDEF RGFW_windowMinimizedfunc RGFW_setWindowMinimizedCallback(RGFW_windowMinimizedfunc func);
/*! set call back for when window is restored. Returns the previous callback function (if it was set) */
RGFWDEF RGFW_windowRestoredfunc RGFW_setWindowRestoredCallback(RGFW_windowRestoredfunc func);
/*! set callback for when the DPI changes. Returns previous callback function (if it was set)  */
RGFWDEF RGFW_scaleUpdatedfunc RGFW_setScaleUpdatedCallback(RGFW_scaleUpdatedfunc func);
/*! set callback for touch events (iOS). Returns previous callback function (if it was set) */
RGFWDEF RGFW_touchfunc RGFW_setTouchCallback(RGFW_touchfunc func);
/** @} */

/** * @defgroup graphics_API
* @{ */

typedef void (*RGFW_proc)(void); /* function pointer equivalent of void* */
		#ifdef RGFW_OPENGL
			RGFW_gfxContext ctx;
			RGFW_gfxContextType gfxType;
		#endif
	};

#elif defined(RGFW_IOS)

	struct RGFW_nativeImage {
		RGFW_format format;
	};

	#ifdef RGFW_OPENGL
		struct RGFW_glContext { void* ctx; };
	#endif

	struct RGFW_window_src {
		void* window; /* UIWindow* */
		void* view; /* UIView* */
		void* viewController; /* UIViewController* */
		void* windowScene; /* UIWindowScene* */
		#ifdef RGFW_OPENGL
			RGFW_gfxContext ctx;
			RGFW_gfxContextType gfxType;
		#endif
	};

#elif defined(RGFW_WASM)

	#include <emscripten/html5.h>
	#include <emscripten/key_codes.h>

	struct RGFW_nativeImage  {
    RGFW_mouse* hiddenMouse;
    RGFW_event events[RGFW_MAX_EVENTS];
	RGFW_bool queueEvents;
	RGFW_bool polledEvents;

    u32 apiKeycodes[RGFW_keyLast];
	#if defined(RGFW_X11) || defined(RGFW_WAYLAND)
		u8 keycodes[256];
	#elif defined(RGFW_WINDOWS)
		u8 keycodes[512];
	#elif defined(RGFW_MACOS)
		u8 keycodes[128];
	#elif defined(RGFW_IOS)
		u8 keycodes[128];
	#elif defined(RGFW_WASM)
		u8 keycodes[256];
	#endif

    const char* className;
    #endif

    #ifdef RGFW_MACOS
    void* NSApp;
	void* customViewClasses[2]; /* NSView and NSOpenGLView  */
	void* customWindowDelegateClass;
    #endif

    #ifdef RGFW_IOS
    void* customViewClass; /* UIView subclass for touch handling */
    #endif

	#ifdef RGFW_OPENGL
		RGFW_window* current;
	#endif
	#ifdef RGFW_EGL
RGFW_CALLBACK_DEFINE(mouseScroll, MouseScroll)
#define RGFW_mouseScrollCallback(w, x, y) if (RGFW_mouseScrollCallbackSrc) RGFW_mouseScrollCallbackSrc(w, x, y);

RGFW_CALLBACK_DEFINE(scaleUpdated, ScaleUpdated)
#define RGFW_scaleUpdatedCallback(w, scaleX, scaleY) if (RGFW_scaleUpdatedCallbackSrc) RGFW_scaleUpdatedCallbackSrc(w, scaleX, scaleY);

RGFW_CALLBACK_DEFINE(touch, Touch)
#define RGFW_touchCallback(w, type, payload) if (RGFW_touchCallbackSrc) RGFW_touchCallbackSrc(w, type, payload);
RGFW_CALLBACK_DEFINE(debug, Debug)
#define RGFW_debugCallback(type, err, msg) if (RGFW_debugCallbackSrc) RGFW_debugCallbackSrc(type, err, msg);
#undef RGFW_CALLBACK_DEFINE

#ifdef RGFW_DEBUG
#include <stdio.h>
#ifdef RGFW_MACOS
	/*NOTE: another OpenGL/setFlags related hack, this because OSX the 'view' class must be setup after the NSOpenGL view is made AND after setFlags happens */
	RGFW_osx_initView(win);
#endif

	RGFW_window_setMouseDefault(win);
	RGFW_window_setName(win, name);
	if (!(flags & RGFW_windowHide)) {
		RGFW_window_show(win);
	}

	RGFW_sendDebugInfo(RGFW_typeInfo, RGFW_infoWindow, "a new  window was created");
	return ret;
void* RGFW_window_getView_OSX(RGFW_window* win) { RGFW_UNUSED(win); return NULL; }
void RGFW_window_setLayer_OSX(RGFW_window* win, void* layer) { RGFW_UNUSED(win); RGFW_UNUSED(layer); }
void* RGFW_getLayer_OSX(void) { return NULL; }
void* RGFW_window_getWindow_OSX(RGFW_window* win) { RGFW_UNUSED(win); return NULL; }
#endif

#ifndef RGFW_IOS
void* RGFW_window_getWindow_iOS(RGFW_window* win) { RGFW_UNUSED(win); return NULL; }
void* RGFW_window_getView_iOS(RGFW_window* win) { RGFW_UNUSED(win); return NULL; }
void* RGFW_window_getViewController_iOS(RGFW_window* win) { RGFW_UNUSED(win); return NULL; }
void* RGFW_window_getWindowScene_iOS(RGFW_window* win) { RGFW_UNUSED(win); return NULL; }
#endif
void RGFW_setBit(u32* var, u32 mask, RGFW_bool set) {
	if (set) *var |=  mask;
	else     *var &= ~mask;
}

void RGFW_window_center(RGFW_window* win) {

    RGFW_sendDebugInfo(RGFW_typeError, RGFW_errX11, buf);
    _RGFW->x11Error = ev;
    return 0;
}

void RGFW_XCreateWindow (XVisualInfo visual, const char* name, RGFW_windowFlags flags, RGFW_window* win) {
	i64 event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | FocusChangeMask | LeaveWindowMask | EnterWindowMask | ExposureMask; /*!< X11 events accepted */

    /* make X window attrubutes */
	XSetWindowAttributes swa;
    RGFW_MEMSET(&swa, 0, sizeof(swa));

/*
	End of MaOS defines
*/

/*
    iOS defines
*/

#ifdef RGFW_IOS
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>

#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#define abi_objc_msgSend_fpret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#endif

#define NSAlloc(nsclass) objc_msgSend_id((id)nsclass, sel_registerName("alloc"))
#define objc_msgSend_id(x, y)                 ((id (*)(id, SEL))objc_msgSend) ((id)(x), (SEL)y)
#define objc_msgSend_uint(x, y)               ((unsigned long (*)(id, SEL))objc_msgSend) ((id)(x), (SEL)y)
#define objc_msgSend_void(x, y)               ((void (*)(id, SEL))objc_msgSend) ((id)(x), (SEL)y)
#define objc_msgSend_void_id(x, y, z)         ((void (*)(id, SEL, id))objc_msgSend) ((id)x, (SEL)y, (id)z)
#define objc_msgSend_void_bool(x, y, z)       ((void (*)(id, SEL, BOOL))objc_msgSend)  ((id)(x), (SEL)y, (BOOL)z)

static inline id NSString_stringWithUTF8String_iOS(const char* str) {
    return ((id(*)(id, SEL, const char*))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), str);
}
static inline const char* NSString_UTF8String_iOS(id str) {
    return ((const char* (*)(id, SEL)) objc_msgSend)((id)str, sel_registerName("UTF8String"));
}

void RGFW_initKeycodesPlatform(void) {
    /* No physical key mapping on iOS for now */
    RGFW_MEMSET(_RGFW->keycodes, 0, sizeof(_RGFW->keycodes));
}

i32 RGFW_initPlatform(void) {
    /* allocate a custom UIView subclass for touch handling */
    if (_RGFW && _RGFW->customViewClass == NULL) {
        Class super = objc_getClass("UIView");
        Class cls = objc_allocateClassPair(super, "RGFWTouchView", 0);
        if (cls) {
            class_addIvar(cls, "RGFW_window", sizeof(RGFW_window*), sizeof(RGFW_window*), "^v");
            /* method signatures: v@:@@ (self, _cmd, touches, event) */
            extern void RGFW__iosTouchesBegan(id self, SEL _cmd, id touches, id event);
            extern void RGFW__iosTouchesMoved(id self, SEL _cmd, id touches, id event);
            extern void RGFW__iosTouchesEnded(id self, SEL _cmd, id touches, id event);
            extern void RGFW__iosTouchesCancelled(id self, SEL _cmd, id touches, id event);
            class_addMethod(cls, sel_registerName("touchesBegan:withEvent:"), (IMP)RGFW__iosTouchesBegan, "v@:@@");
            class_addMethod(cls, sel_registerName("touchesMoved:withEvent:"), (IMP)RGFW__iosTouchesMoved, "v@:@@");
            class_addMethod(cls, sel_registerName("touchesEnded:withEvent:"), (IMP)RGFW__iosTouchesEnded, "v@:@@");
            class_addMethod(cls, sel_registerName("touchesCancelled:withEvent:"), (IMP)RGFW__iosTouchesCancelled, "v@:@@");
            objc_registerClassPair(cls);
            _RGFW->customViewClass = (void*)cls;
        }
    }
    return 0;
}

RGFW_window* RGFW_createWindowPlatform(const char* name, RGFW_windowFlags flags, RGFW_window* win) {
    RGFW_UNUSED(flags);

    id screen = objc_msgSend_id((id)objc_getClass("UIScreen"), sel_registerName("mainScreen"));
    CGRect bounds = ((CGRect(*)(id, SEL))objc_msgSend)(screen, sel_registerName("bounds"));

    if (win->w <= 0 || win->h <= 0) {
        win->x = 0; win->y = 0;
        win->w = (i32)bounds.size.width;
        win->h = (i32)bounds.size.height;
    }

    id window = NSAlloc(objc_getClass("UIWindow"));
    window = ((id(*)(id, SEL, CGRect))objc_msgSend)(window, sel_registerName("initWithFrame:"), bounds);

    id vc = objc_msgSend_id((id)objc_getClass("UIViewController"), sel_registerName("new"));
    if (name) objc_msgSend_void_id(vc, sel_registerName("setTitle:"), NSString_stringWithUTF8String_iOS(name));

    objc_msgSend_void_id(window, sel_registerName("setRootViewController:"), vc);
    id view = NULL;
    if (_RGFW && _RGFW->customViewClass) {
        view = ((id(*)(id, SEL, CGRect))objc_msgSend)(NSAlloc((Class)_RGFW->customViewClass), sel_registerName("initWithFrame:"), bounds);
        /* back-reference to RGFW_window for event routing */
        object_setInstanceVariable(view, "RGFW_window", win);
        ((void(*)(id,SEL,BOOL))objc_msgSend)(view, sel_registerName("setMultipleTouchEnabled:"), YES);
        ((void(*)(id,SEL,BOOL))objc_msgSend)(view, sel_registerName("setUserInteractionEnabled:"), YES);
        objc_msgSend_void_id(vc, sel_registerName("setView:"), view);
    } else {
        view = objc_msgSend_id(vc, sel_registerName("view"));
        ((void(*)(id, SEL, CGRect))objc_msgSend)(view, sel_registerName("setFrame:"), bounds);
    }

    objc_msgSend_void(window, sel_registerName("makeKeyAndVisible"));

    win->src.window = window;
    win->src.viewController = vc;
    win->src.view = view;
    win->src.windowScene = objc_msgSend_id(window, sel_registerName("windowScene"));
    return win;
}

void RGFW_deinitPlatform(void) { }

void RGFW_window_closePlatform(RGFW_window* win) {
    if (win && win->src.window) {
        objc_msgSend_void_bool(win->src.window, sel_registerName("setHidden:"), YES);
        win->src.window = NULL;
        win->src.view = NULL;
        win->src.viewController = NULL;
        win->src.windowScene = NULL;
    }
}

/* Raw handle accessors */
void* RGFW_window_getWindow_iOS(RGFW_window* win) { return win ? win->src.window : NULL; }
void* RGFW_window_getView_iOS(RGFW_window* win) { return win ? win->src.view : NULL; }
void* RGFW_window_getViewController_iOS(RGFW_window* win) { return win ? win->src.viewController : NULL; }
void* RGFW_window_getWindowScene_iOS(RGFW_window* win) { return win ? win->src.windowScene : NULL; }

/* Touch handling (UIView subclass methods) */
static void RGFW__iosTouchesDispatch(id self, id touchesSet, RGFW_eventType evtType) {
    RGFW_window* win = NULL; object_getInstanceVariable(self, "RGFW_window", (void**)&win);
    if (win == NULL) return;
    if (!(win->internal.enabledEvents & RGFW_touchEventsFlag)) return;

    id array = objc_msgSend_id(touchesSet, sel_registerName("allObjects"));
    unsigned long count = objc_msgSend_uint(array, sel_registerName("count"));

    RGFW_event e; RGFW_MEMSET(&e, 0, sizeof(e));
    e.type = evtType; e.common.win = win;
    e.touch.type = evtType; e.touch.win = win;
    e.touch.count = (u8)((count > 10) ? 10 : count);
    for (u8 i = 0; i < e.touch.count; ++i) {
        id touch = ((id(*)(id, SEL, unsigned long))objc_msgSend)(array, sel_registerName("objectAtIndex:"), (unsigned long)i);
        CGPoint p = ((CGPoint(*)(id, SEL, id))abi_objc_msgSend_stret)(touch, sel_registerName("locationInView:"), self);
        e.touch.touches[i].id = (u64)(uintptr_t)touch;
        e.touch.touches[i].x = (i32)p.x;
        e.touch.touches[i].y = (i32)p.y;
    }

    RGFW_eventQueuePush(&e);
    RGFW_touchCallback(win, evtType, (const void*)&e.touch);
}

void RGFW__iosTouchesBegan(id self, SEL _cmd, id touches, id event) { RGFW_UNUSED(_cmd); RGFW_UNUSED(event); RGFW__iosTouchesDispatch(self, touches, RGFW_touchBegan); }
void RGFW__iosTouchesMoved(id self, SEL _cmd, id touches, id event) { RGFW_UNUSED(_cmd); RGFW_UNUSED(event); RGFW__iosTouchesDispatch(self, touches, RGFW_touchMoved); }
void RGFW__iosTouchesEnded(id self, SEL _cmd, id touches, id event) { RGFW_UNUSED(_cmd); RGFW_UNUSED(event); RGFW__iosTouchesDispatch(self, touches, RGFW_touchEnded); }
void RGFW__iosTouchesCancelled(id self, SEL _cmd, id touches, id event) { RGFW_UNUSED(_cmd); RGFW_UNUSED(event); RGFW__iosTouchesDispatch(self, touches, RGFW_touchCancelled); }

/* Attach RGFW UIWindow to first UIWindowScene and make visible */
void RGFW_attachWindowToFirstScene_iOS(RGFW_window* win) {
    if (!win || !win->src.window) return;
    id app = objc_msgSend_id((id)objc_getClass("UIApplication"), sel_registerName("sharedApplication"));
    id scenesSet = objc_msgSend_id(app, sel_registerName("connectedScenes"));
    id scenes = objc_msgSend_id(scenesSet, sel_registerName("allObjects"));
    unsigned long count = objc_msgSend_uint(scenes, sel_registerName("count"));
    if (count == 0) return;
    id sc = ((id(*)(id,SEL,unsigned long))objc_msgSend)(scenes, sel_registerName("objectAtIndex:"), 0UL);
    ((void(*)(id,SEL,id))objc_msgSend)((id)win->src.window, sel_registerName("setWindowScene:"), sc);
    objc_msgSend_void((id)win->src.window, sel_registerName("makeKeyAndVisible"));
}

/* C-only simple entry using runtime */
static int (*RGFW_userMain_iOS)(void) = NULL;
static void RGFW_imp_rgfwStart(id self, SEL _cmd) { RGFW_UNUSED(self); RGFW_UNUSED(_cmd); if (RGFW_userMain_iOS) RGFW_userMain_iOS(); }
static id RGFW_imp_app_config(id self, SEL _cmd, id application, id connectingSceneSession, id options) {
    RGFW_UNUSED(self); RGFW_UNUSED(_cmd); RGFW_UNUSED(application); RGFW_UNUSED(options);
    id role = objc_msgSend_id(connectingSceneSession, sel_registerName("role"));
    id cfg = ((id(*)(id,SEL,id,id))objc_msgSend)((id)objc_getClass("UISceneConfiguration"), sel_registerName("configurationWithName:sessionRole:"), NSString_stringWithUTF8String_iOS("Default Configuration"), role);
    Class sceneDel = objc_getClass("RGFW_CSceneDelegate");
    if (sceneDel) ((void(*)(id,SEL,Class))objc_msgSend)(cfg, sel_registerName("setDelegateClass:"), sceneDel);
    return cfg;
}
static void RGFW_imp_scene_willConnect(id self, SEL _cmd, id scene, id session, id options) {
    RGFW_UNUSED(_cmd); RGFW_UNUSED(session); RGFW_UNUSED(options);
    ((void(*)(id,SEL,SEL,id,bool))objc_msgSend)(self, sel_registerName("performSelectorOnMainThread:withObject:waitUntilDone:"), sel_registerName("rgfwStart"), (id)NULL, false);
}

int RGFW_iOS_run(int argc, char** argv, int (*userMain)(void)) {
    RGFW_userMain_iOS = userMain;

    /* Scene delegate */
    Class sceneSuper = objc_getClass("UIResponder");
    Class sceneCls = objc_allocateClassPair(sceneSuper, "RGFW_CSceneDelegate", 0);
    if (sceneCls) {
        class_addMethod(sceneCls, sel_registerName("scene:willConnectToSession:options:"), (IMP)RGFW_imp_scene_willConnect, "v@:@@@");
        class_addMethod(sceneCls, sel_registerName("rgfwStart"), (IMP)RGFW_imp_rgfwStart, "v@:");
        class_addIvar(sceneCls, "window", sizeof(id), sizeof(id), "@");
        Protocol* pScene = objc_getProtocol("UIWindowSceneDelegate"); if (pScene) class_addProtocol(sceneCls, pScene);
        objc_registerClassPair(sceneCls);
    }

    /* App delegate */
    Class appSuper = objc_getClass("UIResponder");
    Class appCls = objc_allocateClassPair(appSuper, "RGFW_CAppDelegate", 0);
    if (appCls) {
        class_addMethod(appCls, sel_registerName("application:configurationForConnectingSceneSession:options:"), (IMP)RGFW_imp_app_config, "@@:@@@");
        Protocol* pApp = objc_getProtocol("UIApplicationDelegate"); if (pApp) class_addProtocol(appCls, pApp);
        objc_registerClassPair(appCls);
    }

    extern int UIApplicationMain(int, char**, void*, void*);
    return UIApplicationMain(argc, argv, NULL, NSString_stringWithUTF8String_iOS("RGFW_CAppDelegate"));
}


/* Basic window management (stubs) */
void RGFW_pollEvents(void) {
    RGFW_resetPrevState();
    /* pump runloop briefly */
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
}

void RGFW_window_setName(RGFW_window* win, const char* name) {
    if (!win || !name) return;
    if (win->src.viewController)
        objc_msgSend_void_id(win->src.viewController, sel_registerName("setTitle:"), NSString_stringWithUTF8String_iOS(name));
}

void RGFW_window_move(RGFW_window* win, i32 x, i32 y) { RGFW_ASSERT(win != NULL); win->x = x; win->y = y; }
void RGFW_window_resize(RGFW_window* win, i32 w, i32 h) {
    RGFW_ASSERT(win != NULL);
    win->w = w; win->h = h;
    if (win->src.view) {
        ((void(*)(id, SEL, CGRect))objc_msgSend)(win->src.view, sel_registerName("setFrame:"), (CGRect){{0,0},{(double)w,(double)h}});
    }
}

void RGFW_window_focus(RGFW_window* win) { RGFW_UNUSED(win); }
void RGFW_window_raise(RGFW_window* win) { RGFW_UNUSED(win); }

void RGFW_window_maximize(RGFW_window* win) {
    RGFW_ASSERT(win != NULL);
    id screen = objc_msgSend_id((id)objc_getClass("UIScreen"), sel_registerName("mainScreen"));
    CGRect bounds = ((CGRect(*)(id, SEL))objc_msgSend)(screen, sel_registerName("bounds"));
    RGFW_window_move(win, 0, 0);
    RGFW_window_resize(win, (i32)bounds.size.width, (i32)bounds.size.height);
}

void RGFW_window_setFullscreen(RGFW_window* win, RGFW_bool fullscreen) {
    RGFW_ASSERT(win != NULL);
    RGFW_setBit(&win->internal.flags, RGFW_windowFullscreen, fullscreen);
    if (fullscreen) RGFW_window_maximize(win);
}

void RGFW_window_minimize(RGFW_window* win) { RGFW_UNUSED(win); }
void RGFW_window_restore(RGFW_window* win) { RGFW_UNUSED(win); }
void RGFW_window_setFloating(RGFW_window* win, RGFW_bool floating) { RGFW_UNUSED(win); RGFW_UNUSED(floating); }
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border) { RGFW_UNUSED(win); RGFW_UNUSED(border); }

void RGFW_window_hide(RGFW_window* win) { if (win && win->src.window) objc_msgSend_void_bool(win->src.window, sel_registerName("setHidden:"), YES); }
void RGFW_window_show(RGFW_window* win) { if (win && win->src.window) objc_msgSend_void_bool(win->src.window, sel_registerName("setHidden:"), NO); }

RGFW_bool RGFW_window_isHidden(RGFW_window* win) {
    if (!win || !win->src.window) return RGFW_FALSE;
    return (RGFW_bool)((BOOL (*)(id, SEL))objc_msgSend)(win->src.window, sel_registerName("isHidden"));
}

void RGFW_window_setOpacity(RGFW_window* win, u8 opacity) {
    if (!win || !win->src.window) return;
    double alpha = (double)opacity / 255.0;
    ((void (*)(id, SEL, double))objc_msgSend)(win->src.window, sel_registerName("setAlpha:"), alpha);
}

/* Mouse stubs (iOS does not have a cursor) */
RGFW_bool RGFW_getGlobalMouse(i32* x, i32* y) { RGFW_UNUSED(x); RGFW_UNUSED(y); return RGFW_FALSE; }
void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show) { RGFW_UNUSED(win); RGFW_window_showMouseFlags(win, show); }
RGFW_bool RGFW_window_isMouseHidden(RGFW_window* win); /* declared generically */
void RGFW_window_moveMouse(RGFW_window* win, i32 x, i32 y) { RGFW_UNUSED(win); RGFW_UNUSED(x); RGFW_UNUSED(y); }
RGFW_mouse* RGFW_loadMouse(u8* data, i32 w, i32 h, RGFW_format format) { RGFW_UNUSED(data); RGFW_UNUSED(w); RGFW_UNUSED(h); RGFW_UNUSED(format); return NULL; }
void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse) { RGFW_UNUSED(win); RGFW_UNUSED(mouse); }
RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 mouse) { RGFW_UNUSED(win); RGFW_UNUSED(mouse); return RGFW_FALSE; }
RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win) { return RGFW_window_setMouseStandard(win, RGFW_mouseNormal); }
void RGFW_freeMouse(RGFW_mouse* mouse) { RGFW_UNUSED(mouse); }

/* Surface helpers (RAM blit only) */
RGFW_bool RGFW_createSurfacePtr(u8* data, i32 w, i32 h, RGFW_format format, RGFW_surface* surface) {
    surface->data = data;
    surface->w = w;
    surface->h = h;
    surface->format = format;
    surface->native.format = RGFW_formatRGBA8;
    return RGFW_TRUE;
}
void RGFW_surface_freePtr(RGFW_surface* surface) { RGFW_UNUSED(surface); }

/* Clipboard via UIPasteboard */
RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity) {
    id pb = objc_msgSend_id((id)objc_getClass("UIPasteboard"), sel_registerName("generalPasteboard"));
    id ns = objc_msgSend_id(pb, sel_registerName("string"));
    if (!ns) return -1;
    const char* cstr = NSString_UTF8String_iOS(ns);
    size_t len = 0; while (cstr[len] != '\0') len++;
    if (str) {
        if (strCapacity < (len + 1)) return 0;
        RGFW_MEMCPY(str, cstr, len);
        str[len] = '\0';
    }
    return (RGFW_ssize_t)len;
}

void RGFW_writeClipboard(const char* text, u32 textLen) {
    RGFW_UNUSED(textLen);
    id pb = objc_msgSend_id((id)objc_getClass("UIPasteboard"), sel_registerName("generalPasteboard"));
    objc_msgSend_void_id(pb, sel_registerName("setString:"), NSString_stringWithUTF8String_iOS(text));
}

/* Monitors (single screen) */
#ifndef RGFW_NO_MONITOR
RGFW_monitor* RGFW_getMonitors(size_t* len) {
    static RGFW_monitor monitors[1];
    RGFW_monitor mon = {0};
    id screen = objc_msgSend_id((id)objc_getClass("UIScreen"), sel_registerName("mainScreen"));
    CGRect bounds = ((CGRect(*)(id, SEL))objc_msgSend)(screen, sel_registerName("bounds"));
    double scale = ((double (*)(id, SEL))abi_objc_msgSend_fpret)(screen, sel_registerName("scale"));
    mon.x = 0; mon.y = 0; mon.mode.w = (i32)bounds.size.width; mon.mode.h = (i32)bounds.size.height; mon.mode.refreshRate = 60; mon.mode.red = 8; mon.mode.green = 8; mon.mode.blue = 8;
    mon.scaleX = (float)scale; mon.scaleY = (float)scale; mon.pixelRatio = (float)scale;
    monitors[0] = mon; if (len) *len = 1; return monitors;
}

RGFW_monitor RGFW_getPrimaryMonitor(void) { size_t l; return RGFW_getMonitors(&l)[0]; }
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win) { RGFW_UNUSED(win); return RGFW_getPrimaryMonitor(); }
RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request) { RGFW_UNUSED(mon); RGFW_UNUSED(mode); RGFW_UNUSED(request); return RGFW_FALSE; }
#endif

/* Timing helpers */
void RGFW_waitForEvent(i32 waitMS) { CFRunLoopRunInMode(kCFRunLoopDefaultMode, (CFTimeInterval)waitMS / 1000.0, true); }

/* OpenGL stubs (OpenGL ES via EAGL not implemented yet) */
#ifdef RGFW_OPENGL
RGFW_bool RGFW_window_createContextPtr_OpenGL(RGFW_window* win, RGFW_glContext* ctx, RGFW_glHints* hints) { RGFW_UNUSED(win); RGFW_UNUSED(ctx); RGFW_UNUSED(hints); return RGFW_FALSE; }
void RGFW_window_deleteContextPtr_OpenGL(RGFW_window* win, RGFW_glContext* ctx) { RGFW_UNUSED(win); RGFW_UNUSED(ctx); }
void RGFW_window_makeCurrentContext_OpenGL(RGFW_window* win) { RGFW_UNUSED(win); }
void* RGFW_getCurrentContext_OpenGL(void) { return NULL; }
void RGFW_window_swapBuffers_OpenGL(RGFW_window* win) { RGFW_UNUSED(win); }
void RGFW_window_swapInterval_OpenGL(RGFW_window* win, i32 swapInterval) { RGFW_UNUSED(win); RGFW_UNUSED(swapInterval); }
RGFW_bool RGFW_extensionSupportedPlatform_OpenGL(const char* extension, size_t len) { RGFW_UNUSED(extension); RGFW_UNUSED(len); return RGFW_FALSE; }
RGFW_proc RGFW_getProcAddress_OpenGL(const char* procname) { RGFW_UNUSED(procname); return NULL; }
#endif

u8 RGFW_rgfwToKeyChar(u32 rgfw_keycode) { return (u8)rgfw_keycode; }

/* Additional window stubs to satisfy API */
void RGFW_window_setMinSize(RGFW_window* win, i32 w, i32 h) { RGFW_UNUSED(win); RGFW_UNUSED(w); RGFW_UNUSED(h); }
void RGFW_window_setMaxSize(RGFW_window* win, i32 w, i32 h) { RGFW_UNUSED(win); RGFW_UNUSED(w); RGFW_UNUSED(h); }
RGFW_bool RGFW_window_setIconEx(RGFW_window* win, u8* data, i32 w, i32 h, RGFW_format format, RGFW_icon type) { RGFW_UNUSED(win); RGFW_UNUSED(data); RGFW_UNUSED(w); RGFW_UNUSED(h); RGFW_UNUSED(format); RGFW_UNUSED(type); return RGFW_FALSE; }
RGFW_bool RGFW_window_isMinimized(RGFW_window* win) { RGFW_UNUSED(win); return RGFW_FALSE; }
RGFW_bool RGFW_window_isMaximized(RGFW_window* win) { RGFW_UNUSED(win); return RGFW_FALSE; }
void RGFW_captureCursor(RGFW_window* win) { RGFW_UNUSED(win); }
void RGFW_releaseCursor(RGFW_window* win) { RGFW_UNUSED(win); }

#endif /* RGFW_IOS */

/*
	WASM defines
*/

#ifdef RGFW_WASM
EM_BOOL Emscripten_on_resize(int eventType, const EmscriptenUiEvent* E, void* userData) {
	RGFW_UNUSED(eventType); RGFW_UNUSED(userData);
