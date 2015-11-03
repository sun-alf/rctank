#ifndef __RGBLED_H__
#define __RGBLED_H__

#include <stdint.h>
#include "GpioDevice.h"


// A 4-bytes-based color definition:
//   byte3 is not used (reserved)
//   byte2 is Red intensity (0xFF is max emission, 0x00 == off)
//   byte1 is Green intensity (0xFF is max emission, 0x00 == off)
//   byte0 is Blue intensity (0xFF is max emission, 0x00 == off)
enum class RGB_Color
{
    Black     = 0x00000000,
    Red       = 0x00FF0000,
    Green     = 0x0000FF00,
    Blue      = 0x000000FF,
    White     = 0x00FFFFFF,
};


// This struct provides an access to the components of consts (defined above)
typedef union
{
    uint32_t dw;
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t reserved;
    } color;
} RGB_ColorS;


class RGBLED : private GpioDevice
{
public:

private:
    typedef struct
    {
        int hi_time;
        int low_time;
        int cycle_cnt;
        RGBLED* obj;
        RGB_Color color;
        int mutex_id;
    } _ThreadArgs;

    static void* _Blink(void* args);

    _ThreadArgs _threadArgs;
    uint8_t _R_gpio;
    uint8_t _G_gpio;
    uint8_t _B_gpio;

    void _SetLights(RGB_Color color);

public:
    RGBLED(DeviceID id, uint8_t r_pin, uint8_t g_pin, uint8_t b_pin);
    ~RGBLED();

    RGB_Color GetColor();
    void SetColor(RGB_Color color);

    virtual int GetMutexID();
    virtual void SetMutexID(int id);
    virtual void Switch(bool on);

    // Blink specified count of times; each blink is 1 "on" and 1 "off" state of LED.
    // @arg 'hi_time':
    //     duration of "on" state of LED (per each blink), in milliseconds.
    // @arg 'low_time':
    //     duration of "off" state of LED (per each blink), in milliseconds.
    // @arg 'cycle_cnt':
    //     count of blinks.
    // @arg 'async' (=true by default):
    //     if it is true, Blink() will run in a new thread; current thread will be used otherwise.
    virtual void Blink(int hi_time, int low_time, int cycle_cnt, bool async = true);
};

#endif  // __RGBLED_H__
