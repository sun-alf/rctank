#include <stdio.h>
#include <memory.h>
#include <time.h>
#include "wiringPi.h"

//
//typedef enum {
//    Unused = 0, 
//} PinModes;
#define PIN_UNUSED  0xFF
//static int J8_Header[41];
static int WiringPi_GPIO[29];


int wiringPiSetup()
{
    memset(WiringPi_GPIO, PIN_UNUSED, sizeof(WiringPi_GPIO));
    return piThreadingInit();
}


void pinMode(int pin, int mode)
{
    WiringPi_GPIO[pin] = mode;
}


void digitalWrite(int pin, int value)
{
    if (WiringPi_GPIO[pin] == OUTPUT)
        printf("[%u]Pin (%d) >>> %d\n", clock(), pin, value);
    else
        printf("?! Unable to write to GPIO %d\n", pin);
}
