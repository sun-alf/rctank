#ifndef __TRANSMISSION_H__
#define __TRANSMISSION_H__

#include <stdint.h>
#include "GpioDevice.h"


class Transmission : private GpioDevice
{
public:
    enum class Direction
    {   // Neutral -- the same as 'stop!'
        Backward = 0, Neutral, Forward
    };

private:
    uint8_t _in1_gpio;
    uint8_t _in2_gpio;
    uint8_t _in3_gpio;
    uint8_t _in4_gpio;
    uint8_t _enA_gpio;
    uint8_t _enB_gpio;

public:
    Transmission(DeviceID id, uint8_t in1_pin, uint8_t in2_pin, uint8_t in3_pin, uint8_t in4_pin, uint8_t enA_pin, uint8_t enB_pin);
    ~Transmission();

    virtual int GetMutexID();
    virtual void SetMutexID(int id);
    virtual void Switch(bool on);

    //TODO: add left/right engine power support (0% - stop, 10%, 20%, 100% etc)
    virtual void LeftTrack(Direction drn);
    virtual void RightTrack(Direction drn);

};

#endif  // __TRANSMISSION_H__
