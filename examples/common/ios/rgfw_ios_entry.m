#import <UIKit/UIKit.h>
#import "rgfw_ios_entry.h"
#import "RGFW.h"

static RGFW_iOSEntryFn s_userMain = NULL;

@interface RGFW_SimpleAppDelegate : UIResponder <UIApplicationDelegate>
@end

@interface RGFW_SimpleSceneDelegate : UIResponder <UIWindowSceneDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

int RGFW_iOS_run(int argc, char** argv, RGFW_iOSEntryFn userMain) {
    s_userMain = userMain;
    return UIApplicationMain(argc, argv, nil, NSStringFromClass([RGFW_SimpleAppDelegate class]));
}

@implementation RGFW_SimpleAppDelegate
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application; (void)launchOptions; return YES;
}

- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    (void)application; (void)options;
    UISceneConfiguration* config = [UISceneConfiguration configurationWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
    config.delegateClass = [RGFW_SimpleSceneDelegate class];
    return config;
}
@end

@implementation RGFW_SimpleSceneDelegate

- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions {
    (void)session; (void)connectionOptions;

    // Attach RGFW window (if created) to this scene
    RGFW_window* rgfwWin = NULL;
    if (RGFW_getInfo() != NULL) rgfwWin = RGFW_getRootWindow();
    if (rgfwWin) {
        UIWindow* uiWindow = (__bridge UIWindow*)RGFW_window_getWindow_iOS(rgfwWin);
        if (uiWindow && [scene isKindOfClass:[UIWindowScene class]]) {
            [uiWindow setWindowScene:(UIWindowScene*)scene];
            self.window = uiWindow;
        }
    }

    // Now schedule user main to run once, after scene attachment completes
    static BOOL started = NO;
    if (!started && s_userMain) {
        started = YES;
        dispatch_async(dispatch_get_main_queue(), ^{
            (void)s_userMain();
        });
    }
}

@end
