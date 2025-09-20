#pragma once
#include <stddef.h>

typedef int (*RGFW_iOSEntryFn_c)(void);

int RGFW_iOS_run_c(int argc, char** argv, RGFW_iOSEntryFn_c user_main);

#define RGFW_IOS_C_APP(user_main_fn) \
    int main(int argc, char** argv) { \
        return RGFW_iOS_run_c(argc, argv, user_main_fn); \
    }

