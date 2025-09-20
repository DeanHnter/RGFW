#import <Foundation/Foundation.h>

typedef int (*RGFW_iOSEntryFn)(void);

int RGFW_iOS_run(int argc, char** argv, RGFW_iOSEntryFn userMain);

#define RGFW_IOS_APP(user_main_fn) \
    int main(int argc, char** argv) { \
        @autoreleasepool { \
            return RGFW_iOS_run(argc, argv, user_main_fn); \
        } \
    }

