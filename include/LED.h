#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include "GpioDevice.h"


class LED : private GpioDevice
{
private:
    typedef struct {
        int hi_time;
        int low_time;
        int cycle_cnt;
        uint8_t gpio;
        int mutex_id;
    } _ThreadArgs;

    static void* _Blink(void* args);

    _ThreadArgs _threadArgs;

public:
    LED(DeviceID id, uint8_t pin);
    ~LED();

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

#endif  // __LED_H__
