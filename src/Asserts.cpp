#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#include "Logger.h"
#include "Asserts.h"

#define _ASSERT_GPIO_INVALID  (-1)


static int8_t _assertionGPIO = _ASSERT_GPIO_INVALID;


static void* _BlinkLED(void* dummy)
{
    if (_assertionGPIO != _ASSERT_GPIO_INVALID)  // good case: it's been set up before
    {
        wiringPiSetup();
        pinMode(_assertionGPIO, OUTPUT);

        while (true)
        {
            digitalWrite(_assertionGPIO, LOW);
            delay(400);
            digitalWrite(_assertionGPIO, HIGH);
            delay(400);
        }
    }
    return NULL;
}


void SetAssertionLedGpio(int gpio)
{
    _assertionGPIO = (int8_t) gpio;
}


void DebugAssert(const char* file, const int line, const char* func, bool trap)
{
#if BUILD_MODE == BUILD_ALPHA || BUILD_MODE == BUILD_RELEASE
    piThreadCreate(_BlinkLED, NULL);
#endif  // BUILD_MODE == BUILD_ALPHA || BUILD_MODE == BUILD_RELEASE

    char details[1024];
    snprintf(details, 1024, "%s [%s, %d]", func, file, line);
    Logger::Put(details, Logger::LogLevel::Assert);

#if BUILD_MODE == BUILD_DEBUG
    printf("ASSERTION FAILED: in %s.\n", details);
    while (trap);  // if trap == true, then hang and wait for debugging
#endif  // BUILD_MODE == BUILD_DEBUG

#if BUILD_MODE < BUILD_RELEASE
    quick_exit(EXIT_FAILURE);
#endif  // BUILD_MODE < BUILD_RELEASE    
}
