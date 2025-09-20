#import "RGFWiOSSceneDelegate.h"
#import "TriangleRenderer.h"
#import "RGFW.h"

@interface RGFWiOSSceneDelegate ()
@property (nonatomic, strong) CADisplayLink* displayLink;
@property (nonatomic, strong) TriangleRenderer* renderer;
@property (nonatomic, assign) RGFW_window* rgfwWindow;
@end

@implementation RGFWiOSSceneDelegate

- (void)tick:(CADisplayLink*)link {
    (void)link;
    [self.renderer draw];
}

- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions {
    (void)session; (void)connectionOptions;
    // Create RGFW window (which internally creates a UIWindow + UIViewController)
    self.rgfwWindow = RGFW_createWindow("RGFW iOS Metal", 0, 0, 0, 0, 0);
    UIWindow* uiWindow = (__bridge UIWindow*)RGFW_window_getWindow_iOS(self.rgfwWindow);
    if ([scene isKindOfClass:[UIWindowScene class]]) {
        [uiWindow setWindowScene:(UIWindowScene*)scene];
        self.window = uiWindow;
    } else {
        self.window = uiWindow;
    }

    UIView* view = (__bridge UIView*)RGFW_window_getView_iOS(self.rgfwWindow);
    self.renderer = [[TriangleRenderer alloc] initWithView:view];

    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)sceneDidBecomeActive:(UIScene *)scene { (void)scene; self.displayLink.paused = NO; }
- (void)sceneWillResignActive:(UIScene *)scene { (void)scene; self.displayLink.paused = YES; }
- (void)sceneDidEnterBackground:(UIScene *)scene { (void)scene; self.displayLink.paused = YES; }
- (void)sceneWillEnterForeground:(UIScene *)scene { (void)scene; self.displayLink.paused = NO; }

@end
