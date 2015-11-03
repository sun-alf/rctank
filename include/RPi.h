/*
 It is possible to obtain info about running board in run-time. Nevertheless, it is much easier to
 define everything in const manner rather then adopt the code to run-time data and debug afterwhile.
 All this info (pinout etc) can be selected at compile-time.
*/

#ifndef __RPI_H__
#define __RPI_H__

#include <stdint.h>
#include "ProjectCfg.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


#define RASPBERRY_PI_MODEL_A          0
#define RASPBERRY_PI_MODEL_B_r1       1
#define RASPBERRY_PI_MODEL_B_r2       2
#define RASPBERRY_PI_MODEL_Aplus      3
#define RASPBERRY_PI_MODEL_Bplus      4
#define RASPBERRY_PI_MODEL_B2         5

#ifndef RASPBERRY_PI_MODEL
#error Raspberry Pi Model is not set! (define RASPBERRY_PI_MODEL)
#endif  // RASPBERRY_PI_MODEL

#define RPI_NOT_GPIO    (0xFF)
#define RPI_FALSE       (0x0)
#define RPI_TRUE        (0x1)


typedef struct
{
    uint8_t gpio;
    char  name[7];
} RPi_PinInfo;


// Count of pins on current RPi board
#if RASPBERRY_PI_MODEL == RASPBERRY_PI_MODEL_Aplus
#   define RPI_PIN_COUNT      (40)
#else
#   error Defined Raspberry Pi Model is not supported! (define RASPBERRY_PI_MODEL)
#endif  // RASPBERRY_PI_MODEL == xx

#define RPI_PIN_BITMAP_SIZE   ((RPI_PIN_COUNT / 8) + (RPI_PIN_COUNT % 8 != 0 ? 1 : 0))
extern const int RPi_PinCount;

// Count of pins which can be used as GPIO
extern const int RPi_GpioCount;

// Max number of GPIO; it depends on both RPi board and WiringPi numbering. Remember, this numbering
// is not just 0..29 -- it has gaps! (see spec.)
extern const int RPi_GpioMax;

// Pin mapping for converting a raw numbering (as on board) into WiringPi numbering.
// Indices 1..xx within array are raw number, WiringPi info is inside of an element.
// There is no pin with raw number 0, so don't use index 0 -- it is foo.
extern const RPi_PinInfo RPi_Pin_Map[];


int RPi_GpioExist(uint32_t gpio);
uint32_t RPi_GpioToPin(uint32_t gpio);


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // __RPI_H__
