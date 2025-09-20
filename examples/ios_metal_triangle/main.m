#import <UIKit/UIKit.h>
#import "RGFW.h"

@interface RGFWiOSAppDelegate : UIResponder <UIApplicationDelegate>
@end

int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([RGFWiOSAppDelegate class]));
    }
}

