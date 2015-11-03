#include "RPi.h"


#if RASPBERRY_PI_MODEL == RASPBERRY_PI_MODEL_Aplus
const int RPi_PinCount = 40;
const int RPi_GpioCount = 26;
const int RPi_GpioMax = 29;

const RPi_PinInfo RPi_Pin_Map[] = {
    {RPI_NOT_GPIO, ""},      // 0 -- there is no pin 0 in raw numbering
    {RPI_NOT_GPIO, "3.3V"},  // 1
    {RPI_NOT_GPIO, "5.0V"},  // 2
    {8, ""},                 // 3
    {RPI_NOT_GPIO, "5.0V"},  // 4
    {9, ""},                 // 5
    {RPI_NOT_GPIO, "Gnd"},   // 6
    {7, ""},                 // 7
    {15, ""},                // 8
    {RPI_NOT_GPIO, "Gnd"},   // 9
    {16, ""},                // 10
    {0, ""},                 // 11
    {1, ""},                 // 12
    {2, ""},                 // 13
    {RPI_NOT_GPIO, "Gnd"},   // 14
    {3, ""},                 // 15
    {4, ""},                 // 16
    {RPI_NOT_GPIO, "3.3V"},  // 17
    {5, ""},                 // 18
    {12, ""},                // 19
    {RPI_NOT_GPIO, "Gnd"},   // 20
    {13, ""},                // 21
    {6, ""},                 // 22
    {14, ""},                // 23
    {10, ""},                // 24
    {RPI_NOT_GPIO, "Gnd"},   // 25
    {11, ""},                // 26
    {RPI_NOT_GPIO, "SDA0"},  // 27
    {RPI_NOT_GPIO, "SCL0"},  // 28
    {21, ""},                // 29
    {RPI_NOT_GPIO, "Gnd"},   // 30
    {22, ""},                // 31
    {26, ""},                // 32
    {23, ""},                // 33
    {RPI_NOT_GPIO, "Gnd"},   // 34
    {24, ""},                // 35
    {27, ""},                // 36
    {25, ""},                // 37
    {28, ""},                // 38
    {RPI_NOT_GPIO, "Gnd"},   // 39
    {29, ""},                // 40
};
#else
#   error Defined Raspberry Pi Model is not supported! (define RASPBERRY_PI_MODEL)
#endif  // RASPBERRY_PI_MODEL == xx


int RPi_GpioExist(uint32_t gpio)
{
    int i;
    for (i = 1; i <= RPI_PIN_COUNT; i++)
        if (RPi_Pin_Map[i].gpio == gpio)
            return 1;
    return 0;
}


uint32_t RPi_GpioToPin(uint32_t gpio)
{
    int i;
    for (i = 1; i <= RPI_PIN_COUNT; i++)
        if (RPi_Pin_Map[i].gpio == gpio)
            return i;
    return 0;
}