#ifndef __ASSERTS_H__
#define __ASSERTS_H__

#include <stdio.h>
#include "ProjectCfg.h"


#define __DEBUG_CODETRAP(_condition_, _msg_, _file_, _line_, _function_)  \
    if ((_condition_) == false) \
    { \
        if (_msg_ != NULL) printf("ASSERTION MESSAGE: %s\n", _msg_); \
        DebugAssert(_file_, _line_, _function_, true); \
    }

#define __DEBUG_ASSERT(_condition_, _msg_, _file_, _line_, _function_)  \
    if ((_condition_) == false) \
    { \
        if (_msg_ != NULL) printf("ASSERTION MESSAGE: %s\n", _msg_); \
        DebugAssert(_file_, _line_, _function_, false); \
    }

#define __RELEASE_ASSERT(_condition_, _file_, _line_, _function_)  \
    if ((_condition_) == false) \
        DebugAssert(_file_, _line_, _function_, false);

// Works in DEBUG mode only
#if BUILD_MODE == BUILD_DEBUG
#   define DEBUG_CODETRAP(_condition_, _msg_)  __DEBUG_CODETRAP(_condition_, _msg_, __FILE__, __LINE__, __FUNCTION__)
#else
#   define DEBUG_CODETRAP(_condition_, _msg_)
#endif  // BUILD_MODE == BUILD_DEBUG

// Works in DEBUG mode, in ALPHA mode
// Behaviour differs [slightly] in these modes
#if BUILD_MODE < BUILD_RELEASE
#   define DEBUG_ASSERT(_condition_, _msg_)  __DEBUG_ASSERT(_condition_, _msg_, __FILE__, __LINE__, __FUNCTION__)
#else
#   define DEBUG_ASSERT(_condition_, _msg_)
#endif  // BUILD_MODE < BUILD_RELEASE

// Works in any build mode. In DEBUG mode works as DEBUG_ASSERT
#if BUILD_MODE == BUILD_DEBUG
#   define ASSERT(_condition_, _msg_)  __DEBUG_ASSERT(_condition_, _msg_, __FILE__, __LINE__, __FUNCTION__)
#else
#   define ASSERT(_condition_, _msg_)  __RELEASE_ASSERT(_condition_, __FILE__, __LINE__, __FUNCTION__)
#endif  // BUILD_MODE == BUILD_DEBUG

void DebugAssert(const char* file, const int line, const char* func, bool trap);
void SetAssertionLedGpio(int gpio);

#endif  // __ASSERTS_H__
