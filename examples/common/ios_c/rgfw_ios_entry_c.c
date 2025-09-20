#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdbool.h>

static int (*g_user_main)(void) = 0;

static inline id nsstring(const char* s) {
    return ((id(*)(id, SEL, const char*))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), s);
}

/* Scene delegate method: - (void)rgfwStart */
static void imp_rgfwStart(id self, SEL _cmd) {
    (void)self; (void)_cmd;
    if (g_user_main) g_user_main();
}

/* App delegate: -application:configurationForConnectingSceneSession:options: */
static id imp_app_config(id self, SEL _cmd, id application, id connectingSceneSession, id options) {
    (void)self; (void)_cmd; (void)application; (void)options;
    id role = ((id(*)(id,SEL))objc_msgSend)(connectingSceneSession, sel_registerName("role"));
    id cfg = ((id(*)(id,SEL,id,id))objc_msgSend)((id)objc_getClass("UISceneConfiguration"), sel_registerName("configurationWithName:sessionRole:"), nsstring("Default Configuration"), role);
    Class sceneDel = objc_getClass("RGFW_CSceneDelegate");
    if (sceneDel) ((void(*)(id,SEL,Class))objc_msgSend)(cfg, sel_registerName("setDelegateClass:"), sceneDel);
    return cfg;
}

/* Scene delegate: -scene:willConnectToSession:options: */
static void imp_scene_willConnect(id self, SEL _cmd, id scene, id session, id options) {
    (void)_cmd; (void)session; (void)options;
    /* schedule user main on next run loop to avoid blocking */
    id nsApp = ((id(*)(id,SEL))objc_msgSend)((id)objc_getClass("UIApplication"), sel_registerName("sharedApplication"));
    (void)nsApp; /* just ensures UIApplication is initialized */
    ((void(*)(id,SEL,SEL,id,bool))objc_msgSend)(self, sel_registerName("performSelectorOnMainThread:withObject:waitUntilDone:"), sel_registerName("rgfwStart"), (id)NULL, false);
}

int RGFW_iOS_run_c(int argc, char** argv, int (*user_main)(void)) {
    g_user_main = user_main;

    /* Register Scene delegate */
    Class sceneSuper = objc_getClass("UIResponder");
    Class sceneCls = objc_allocateClassPair(sceneSuper, "RGFW_CSceneDelegate", 0);
    if (sceneCls) {
        class_addMethod(sceneCls, sel_registerName("scene:willConnectToSession:options:"), (IMP)imp_scene_willConnect, "v@:@@@");
        class_addMethod(sceneCls, sel_registerName("rgfwStart"), (IMP)imp_rgfwStart, "v@:");
        /* add window property backing ivar for completeness */
        class_addIvar(sceneCls, "window", sizeof(id), sizeof(id), "@");
        /* adopt UIWindowSceneDelegate protocol */
        Protocol* pScene = objc_getProtocol("UIWindowSceneDelegate");
        if (pScene) class_addProtocol(sceneCls, pScene);
        objc_registerClassPair(sceneCls);
    }

    /* Register App delegate */
    Class appSuper = objc_getClass("UIResponder");
    Class appCls = objc_allocateClassPair(appSuper, "RGFW_CAppDelegate", 0);
    if (appCls) {
        class_addMethod(appCls, sel_registerName("application:configurationForConnectingSceneSession:options:"), (IMP)imp_app_config, "@@:@@@");
        /* adopt UIApplicationDelegate protocol */
        Protocol* pApp = objc_getProtocol("UIApplicationDelegate");
        if (pApp) class_addProtocol(appCls, pApp);
        objc_registerClassPair(appCls);
    }

    /* UIApplicationMain signature guarded to avoid importing UIKit */
    extern int UIApplicationMain(int, char**, void*, void*);
    return UIApplicationMain(argc, argv, NULL, nsstring("RGFW_CAppDelegate"));
}
