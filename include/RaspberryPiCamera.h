#ifndef __RASPBERRY_PI_CAMERA_H__
#define __RASPBERRY_PI_CAMERA_H__

#include <stdint.h>
#include "GpioDevice.h"


class RaspberryPiCamera : private GpioDevice
{
private:
    int _mutexID;
    //int _gigi;

public:
    RaspberryPiCamera(DeviceID id, uint8_t pins[], int pinCnt);
    ~RaspberryPiCamera();

    virtual int GetMutexID();
    virtual void SetMutexID(int id);
    virtual void Switch(bool on);
};

#endif  // __RASPBERRY_PI_CAMERA_H__
