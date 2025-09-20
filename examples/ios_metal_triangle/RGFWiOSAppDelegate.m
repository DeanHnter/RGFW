#import "RGFWiOSAppDelegate.h"
#import "RGFW.h"

@implementation RGFWiOSAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application; (void)launchOptions;
    return YES;
}

- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    (void)application; (void)options;
    UISceneConfiguration* config = [UISceneConfiguration configurationWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
    config.delegateClass = NSClassFromString(@"RGFWiOSSceneDelegate");
    return config;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    (void)application;
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    (void)application;
}

@end
