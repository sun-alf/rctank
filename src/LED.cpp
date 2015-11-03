#include <wiringPi.h>
#include "RPi.h"
#include "LED.h"


void* LED::_Blink(void* args)
{
    LED::_ThreadArgs* threadArgs = (LED::_ThreadArgs*) args;

    piLock(threadArgs->mutex_id);
    for (int i = 0; i < threadArgs->cycle_cnt; i++)
    {
        digitalWrite(threadArgs->gpio, HIGH);
        delay(threadArgs->hi_time);
        digitalWrite(threadArgs->gpio, LOW);
        delay(threadArgs->low_time);
    }

    piUnlock(threadArgs->mutex_id);
    return (void*) 0;
}


LED::LED(DeviceID id, uint8_t pin) : GpioDevice(id)
{
    _threadArgs.gpio = RPi_Pin_Map[pin].gpio;
    pinMode(_threadArgs.gpio, OUTPUT);
}


LED::~LED()
{

}


int LED::GetMutexID()
{
    return _threadArgs.mutex_id;
}


void LED::SetMutexID(int id)
{
    _threadArgs.mutex_id = id;
}


void LED::Switch(bool on)
{
    piLock(_threadArgs.mutex_id);
    if (on)
        digitalWrite(_threadArgs.gpio, HIGH);
    else
        digitalWrite(_threadArgs.gpio, LOW);
    piUnlock(_threadArgs.mutex_id);
}


void LED::Blink(int hi_time, int low_time, int cycle_cnt, bool async)
{
    _threadArgs.hi_time = hi_time;
    _threadArgs.low_time = low_time;
    _threadArgs.cycle_cnt = cycle_cnt;
    if (async)
        piThreadCreate(&LED::_Blink, &_threadArgs);
    else
        LED::_Blink(&_threadArgs);
}
