#include <wiringPi.h>
#include "RPi.h"
#include "Transmission.h"


Transmission::Transmission(DeviceID id, uint8_t in1_pin, uint8_t in2_pin, uint8_t in3_pin, uint8_t in4_pin, uint8_t enA_pin, uint8_t enB_pin) :
        _in1_gpio(RPi_Pin_Map[in1_pin].gpio), _in2_gpio(RPi_Pin_Map[in2_pin].gpio), _in3_gpio(RPi_Pin_Map[in3_pin].gpio),
        _in4_gpio(RPi_Pin_Map[in4_pin].gpio), _enA_gpio(RPi_Pin_Map[enA_pin].gpio), _enB_gpio(RPi_Pin_Map[enB_pin].gpio),
        GpioDevice(id)
{
    pinMode(_in1_gpio, OUTPUT);
    pinMode(_in2_gpio, OUTPUT);
    pinMode(_in3_gpio, OUTPUT);
    pinMode(_in4_gpio, OUTPUT);
    pinMode(_enA_gpio, OUTPUT);
    pinMode(_enB_gpio, OUTPUT);

    //TODO: remove it when throttle support is done
    digitalWrite(_enA_gpio, HIGH);
    digitalWrite(_enB_gpio, HIGH);
}


Transmission::~Transmission()
{

}

int Transmission::GetMutexID()
{
    return 0;
}


void Transmission::SetMutexID(int id)
{
    
}


void Transmission::Switch(bool on)
{
    
}


void Transmission::LeftTrack(Direction drn)
{
    switch (drn)
    {
    case Direction::Backward:
        digitalWrite(_in1_gpio, HIGH);
        digitalWrite(_in2_gpio, LOW);
        break;
    case Direction::Forward:
        digitalWrite(_in1_gpio, LOW);
        digitalWrite(_in2_gpio, HIGH);
        break;
    case Direction::Neutral:
    default:
        digitalWrite(_in1_gpio, LOW);
        digitalWrite(_in2_gpio, LOW);
        break;
    }
}


void Transmission::RightTrack(Direction drn)
{
    switch (drn)
    {
    case Direction::Backward:
        digitalWrite(_in3_gpio, HIGH);
        digitalWrite(_in4_gpio, LOW);
        break;
    case Direction::Forward:
        digitalWrite(_in3_gpio, LOW);
        digitalWrite(_in4_gpio, HIGH);
        break;
    case Direction::Neutral:
    default:
        digitalWrite(_in3_gpio, LOW);
        digitalWrite(_in4_gpio, LOW);
        break;
    }
}
