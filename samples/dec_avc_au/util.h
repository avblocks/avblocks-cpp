#pragma once

#if defined(_WIN32) || defined(_WIN64)
    // Windows
    #include "windows/util.h"
#elif defined(__APPLE__)
    // macOS
    #include "darwin/util.h"
#elif defined (__linux__)
    // Linux
    #include "linux/util.h"
#endif