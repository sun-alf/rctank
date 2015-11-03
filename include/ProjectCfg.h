#ifndef __PROJECT_CFG_H__
#define __PROJECT_CFG_H__

#define RASPBERRY_PI_MODEL  3  // model A+

/*
 BUILD_RELEASE:
     There is no any conventional peripheria. The only Log/CrashDump can be used for FA. Firmware
     tries to survive at any cost -- it writes down any error and skips failed commands, or even
     it restarts itself in case of a fatal error.
     FA scenario:
     1. Switch off RC-toy and disassemble it.
     2. Eject microSD and look into Log/CrashDump OR attach Display+Keyboard.
     3. Profit! Do FA using Log/CrashDump, update Firmware.

 BUILD_ALPHA:
     RC-toy moves and executes commands but isn't assembled completely; it's under development.
     Mostly like there is no way to attach Display without cutting power. Hense, Firmware should
     not be stuck anywhere at point of error. But it should save as much debug info as possible
     before system shutdown.
     FA scenario: switch off RC-toy, attach peripheria and continue the development.

 BUILD_DEBUG:
     There are Display+Keyboard attached or even possibility to attach a debugger. Firmware can
     be stuck at any error point and/or print assertion info to everywhere. Assertion info is
     something pointing to where trouble happened or detected (file name, line number etc).
     No special preparations for FA is need.
*/
#define BUILD_RELEASE        5
#define BUILD_ALPHA          3
#define BUILD_DEBUG          0

#ifndef BUILD_MODE
#   define BUILD_MODE BUILD_DEBUG
#endif  // BUILD_MODE


#define BUILD_WINDOWS        0  // actually means VS C++ compiler =)
#define BUILD_LINUX          1

#ifndef BUILD_PLATFORM
#   define BUILD_PLATFORM BUILD_WINDOWS
#endif  // BUILD_PLATFORM

#if BUILD_PLATFORM == BUILD_WINDOWS
#   define quick_exit(x)  exit(x)
#   define snprintf  _snprintf
#endif  // BUILD_PLATFORM == BUILD_WINDOWS


#pragma warning (disable : 4482)    // non-standard enum access 'enum_name::member_name'

#endif  // __PROJECT_CFG_H__
