#include <wiringPi.h>
#include "RPi.h"
#include "RGBLED.h"


void* RGBLED::_Blink(void* args)
{
    RGBLED::_ThreadArgs* threadArgs = (RGBLED::_ThreadArgs*) args;

    piLock(threadArgs->mutex_id);
    for (int i = 0; i < threadArgs->cycle_cnt; i++)
    {
        threadArgs->obj->_SetLights(threadArgs->color);
        delay(threadArgs->hi_time);
        threadArgs->obj->_SetLights(RGB_Color::Black);
        delay(threadArgs->low_time);
    }

    piUnlock(threadArgs->mutex_id);
    return (void*)0;
}


void RGBLED::_SetLights(RGB_Color color)
{
    RGB_ColorS rgbColor;
    rgbColor.dw = (uint32_t) color;

    if (rgbColor.color.red == 0xFF)
        digitalWrite(_R_gpio, HIGH);
    else
        digitalWrite(_R_gpio, LOW);

    if (rgbColor.color.green == 0xFF)
        digitalWrite(_G_gpio, HIGH);
    else
        digitalWrite(_G_gpio, LOW);

    if (rgbColor.color.blue == 0xFF)
        digitalWrite(_B_gpio, HIGH);
    else
        digitalWrite(_B_gpio, LOW);
}


RGBLED::RGBLED(DeviceID id, uint8_t r_pin, uint8_t g_pin, uint8_t b_pin) :
        _R_gpio(RPi_Pin_Map[r_pin].gpio), _G_gpio(RPi_Pin_Map[g_pin].gpio), _B_gpio(RPi_Pin_Map[b_pin].gpio),
        GpioDevice(id)
{
    pinMode(_R_gpio, OUTPUT);
    pinMode(_G_gpio, OUTPUT);
    pinMode(_B_gpio, OUTPUT);
    _threadArgs.obj = this;
    _threadArgs.color = RGB_Color::Red;
}


RGBLED::~RGBLED()
{

}


int RGBLED::GetMutexID()
{
    return _threadArgs.mutex_id;
}


void RGBLED::SetMutexID(int id)
{
    _threadArgs.mutex_id = id;
}


RGB_Color RGBLED::GetColor()
{
    return _threadArgs.color;
}


void RGBLED::SetColor(RGB_Color color)
{
    _threadArgs.color = color;
}


void RGBLED::Switch(bool on)
{
    piLock(_threadArgs.mutex_id);
    if (on)
        _SetLights(_threadArgs.color);
    else
        _SetLights(RGB_Color::Black);
    piUnlock(_threadArgs.mutex_id);
}


void RGBLED::Blink(int hi_time, int low_time, int cycle_cnt, bool async)
{
    _threadArgs.hi_time = hi_time;
    _threadArgs.low_time = low_time;
    _threadArgs.cycle_cnt = cycle_cnt;
    if (async)
        piThreadCreate(&RGBLED::_Blink, &_threadArgs);
    else
        RGBLED::_Blink(&_threadArgs);
}
