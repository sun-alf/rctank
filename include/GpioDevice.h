#ifndef __GPIO_DEVICE_H__
#define __GPIO_DEVICE_H__

typedef unsigned int DeviceID;

class GpioDevice
{
private:
    DeviceID _id;

public:
    GpioDevice(DeviceID id);
    ~GpioDevice();

    DeviceID GetDeviceID();

    virtual int GetMutexID() = 0;
    virtual void SetMutexID(int id) = 0;
    virtual void Switch(bool on) = 0;
};

#endif  // __GPIO_DEVICE_H__
