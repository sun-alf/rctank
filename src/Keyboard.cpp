#include <memory.h>
#include "ProjectCfg.h"

#if BUILD_PLATFORM == BUILD_WINDOWS
#   include <conio.h>
#elif BUILD_PLATFORM == BUILD_LINUX
#   include <linux/input.h>
#   include <fcntl.h>
#   include <unistd.h>
#endif  // BUILD_PLATFORM == BUILD_WINDOWS

#include "Logger.h"
#include "Keyboard.h"


Keyboard::Keyboard()
{
    memset(&_evt, 0, sizeof(_evt));
}


Keyboard::~Keyboard()
{

}


KeyboardEvent* Keyboard::GetEvent()
{
#if BUILD_PLATFORM == BUILD_WINDOWS
    static uint16_t last_key = 0;  // for emulation of continious key pressing

    while(_kbhit() == 0);
    _evt.key = (uint16_t) _getch();
    if (_evt.key == last_key)
        _evt.isDown = true;
    else
        _evt.isDown = false;

    last_key = _evt.key;
#elif BUILD_PLATFORM == BUILD_LINUX

    input_event kbEvent;
    int bytesRead;
    bool listenMore = true;

    while (listenMore)
    {
        listenMore = false;
        if ( (bytesRead = read(_dev_file_desc, &kbEvent, sizeof(input_event))) == sizeof(input_event) )
        {
            if (kbEvent.type == EV_KEY)
            {
                switch (kbEvent.code)
                {
                case KEY_LEFTCTRL:
                case KEY_RIGHTCTRL:
                    _evt.ctrl = (bool) kbEvent.value;
                    _evt.key = KEY_RESERVED;
                    break;
                case KEY_LEFTALT:
                case KEY_RIGHTALT:
                    _evt.alt = (bool) kbEvent.value;
                    _evt.key = KEY_RESERVED;
                    break;
                case KEY_LEFTSHIFT:
                case KEY_RIGHTSHIFT:
                    _evt.shift = (bool) kbEvent.value;
                    _evt.key = KEY_RESERVED;
                    break;
                default:
                    _evt.key = kbEvent.code;
                    _evt.isDown = (bool) kbEvent.value;
                    break;
                }
            }
            else  // not a key event
                listenMore = true;
        }
        else  // bad case
        {
            Logger::PutError(ERR_KBD_READ, Logger::LogLevel::HwFailure);
            memset(&_evt, 0, sizeof(_evt));
        }
    }
#endif  // BUILD_PLATFORM == BUILD_WINDOWS
    return &_evt;
}


Err Keyboard::Init(void* args)
{
    KeyboardInitArgs* init_args = (KeyboardInitArgs*) args;
#if BUILD_PLATFORM == BUILD_LINUX
    // Open Device
    if ((_dev_file_desc = open (init_args->dev_file, O_RDONLY)) == -1)
        return ERR_KBD_OPEN;
#endif  // BUILD_PLATFORM == BUILD_LINUX
    _callback = init_args->callback;
    return ERR_OK;
}


void Keyboard::WaitInputEvent(InputEvent* evt)
{
    KeyboardEvent* keyEvt = GetEvent();

    evt->bt.action = _callback(keyEvt);
    evt->bt.begin = keyEvt->isDown;
}
