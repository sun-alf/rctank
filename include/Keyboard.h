#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "InputSource.h"


typedef struct
{
    uint16_t key;
    //uint16_t flags;  //TODO: optimize me if need
    bool isDown;  // 0 - key is up, 1 - key is down
    bool shift;
    bool ctrl;
    bool alt;
} KeyboardEvent;


typedef struct
{
    char* dev_file;
    uint16_t (*callback)(KeyboardEvent*);
} KeyboardInitArgs;


class Keyboard : private InputSource
{
private:
    KeyboardEvent _evt;
    int _dev_file_desc;
    uint16_t (*_callback)(KeyboardEvent*);

public:
    Keyboard();
    ~Keyboard();

    // Waits for keyboard input (any of events) and returns pointer to a result. It's a blocking method.
    // @return:
    //     pointer to an event description; do not free it.
    KeyboardEvent* GetEvent();

    virtual Err Init(void* args);
    virtual void WaitInputEvent(InputEvent* evt);
};

#endif  // __KEYBOARD_H__
