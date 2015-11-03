#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "InputSource.h"
#include "Keyboard.h"


typedef enum
{
    ACTION_INVALID = -1,
    ACTION_ExitApp = 0,
    ACTION_MoveForward,
    ACTION_MoveBackward,
    ACTION_TurnLeft,
    ACTION_TurnRight,
    ACTION_MovementStop,
    ACTION_LeftTrackForward,
    ACTION_LeftTrackBackward,
    ACTION_RightTrackForward,
    ACTION_RightTrackBackward,
    ACTIONS_TOTAL_COUNT
} ControlActionCode;


class Controller
{
private:
    static const uint8_t _KEY_MODIFIER_SHIFT = 0x01;
    static const uint8_t _KEY_MODIFIER_CTRL  = 0x02;
    static const uint8_t _KEY_MODIFIER_ALT   = 0x04;

    //static const uint16_t _ACTION_EXIT_APP           = 0x0000;
    //static const uint16_t _ACTIONS_TOTAL_COUNT       = 0x0100;

    //typedef enum
    enum class _ProfileSection
    {
        IOSetup = 0, Behavior, Keyboard,
        Undefined  // can be used as "count of values"
    };
    //} _ProfileSection;

    //typedef enum
    enum class _RemoteControlType
    {
        Keyboard = 0, Wifi, BlueTooth, InfraRed, 
        Undefined  // can be used as "count of values"
    };
    //} _RemoteControlType;

    static const int _BindingName_MaxLength = 16;
    typedef char _BindingName[_BindingName_MaxLength];

    typedef struct
    {
        //char name[16];
        uint16_t action;
        uint16_t key;
        //uint16_t code;
        uint8_t mods;
    } _ControlBinding;

    static const char* const _ProfileSectionStr[];
    static const char* const _RemoteControlTypeStr[];
    static _ControlBinding _bindings[];
    static int _bindingsCount;
    static InputSource* _inputSource;
    static _RemoteControlType _inputSourceType;
    static char* _keyboardName;

    static const char* _ProfileSectionToStr(_ProfileSection section);
    static _ProfileSection _StrToProfileSection(const char* str);
    static const char* _RemoteControlTypeToStr(_RemoteControlType rc);
    static _RemoteControlType _StrToRemoteControlType(const char* str);

    static void _AddControlBinding(ControlActionCode action, uint16_t key, uint8_t mods);

    // Converts input text into a parsed info if possible.
    // @arg 'line':
    //     pointer to a buffer with a complete text line from a config-file. The text will be changed
    //     during execution, so make sure you have original copy if need.
    // @arg [IN/OUT] 'currentSection':
    //     a profile section within the parsing line is. If the line is a next section name then 
    //     value pointed by tihs arg will be changed accordingly.
    static void _ParseLine(char* line, _ProfileSection* currentSection, _BindingName* bindingNames);

    static void _InitInputSource();
    static uint16_t __FckConvert(KeyboardEvent* evt);

public:
    static bool Initialize(char* binding_data, char* profile_data);
    static void TakeControl();

    // Allow or suppress (disallow) some commands input source like keybord, WiFi etc.
    // @arg 'source':
    //     textual name of source, i.e. 'keyboard', 'wifi_usb' (see Help.txt).
    // @arg 'allow':
    //     set 'true' to make it enabled, set 'false' to make it disabled.
    //static void AllowInputSource(char* source, bool allow);
    //static void SuppressInputSource(char* source);

    // A static destructor. It frees all allocated heap memory, pulls to 0 all used GPIOs and
    // releases them.
    static void Destroy();
};

#endif  // __CONTROLLER_H__
