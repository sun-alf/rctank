#ifndef __INPUT_SOURCE_H__
#define __INPUT_SOURCE_H__

#include <stdint.h>
#include "Error.h"


typedef union
{
    uint32_t dw;
    struct
    {
        uint32_t action    : 16;    // action code
        uint32_t axis      : 8;     // level on axis if it is used (joystick etc)
        //uint32_t progress  : 1;     // 1 - action is continuous, 0 - otherwise; mutually exclusive with 'begin'
        uint32_t begin     : 1;     // used if 'progress' is 0; 1 - action was just started, 0 - finished
    } bt;
} InputEvent;


class InputSource
{
private:

public:
    InputSource();
    ~InputSource();

    // Possibly some parts of init-n can not be done during construction, so this method can be
    // called at handy moment.
    // @return:
    //     0 - all is OK, error code otherwise.
    virtual Err Init(void* args) = 0;

    virtual void WaitInputEvent(InputEvent* args) = 0;
    //virtual void SetMutexID(int id) = 0;
    //virtual void Switch(bool on) = 0;
};

#endif  // __INPUT_SOURCE_H__
