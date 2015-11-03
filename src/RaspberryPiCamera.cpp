#include "RaspberryPiCamera.h"
#include <stdio.h>  //TEMP:


RaspberryPiCamera::RaspberryPiCamera(DeviceID id, uint8_t pins[], int pinCnt)
        : GpioDevice(id)
{
    
}


RaspberryPiCamera::~RaspberryPiCamera()
{

}


int RaspberryPiCamera::GetMutexID()
{
    return _mutexID;
}


void RaspberryPiCamera::SetMutexID(int id)
{
    _mutexID = id;
}


void RaspberryPiCamera::Switch(bool on)
{
    printf("[%d]piCam: %s\n", GetDeviceID(), on ? "ON" : "OFF");  //TEMP:
}