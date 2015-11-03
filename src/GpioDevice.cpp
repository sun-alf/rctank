#include "GpioDevice.h"


GpioDevice::GpioDevice(DeviceID id) : _id(id)
{

}


GpioDevice::~GpioDevice()
{

}


DeviceID GpioDevice::GetDeviceID()
{
    return _id;
}
