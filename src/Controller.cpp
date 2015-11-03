#include <string.h>
//#include <wiringPi.h>

#include "Error.h"
#include "Asserts.h"
#include "Logger.h"
#include "ParsingUtils.h"
//#include "LED.h"
//#include "RGBLED.h"
//#include "RaspberryPiCamera.h"
#include "Transmission.h"
#include "DeviceManager.h"
#include "Keyboard.h"
#include "Controller.h"


const char* const Controller::_ProfileSectionStr[] =
{
    "IOSETUP", "BEHAVIOR", "KEYBOARD",
};


const char* const Controller::_RemoteControlTypeStr[] =
{
    _ProfileSectionStr[(int)_ProfileSection::Keyboard], "WIFI", "BLUETOOTH", "IR",
};


Controller::_ControlBinding Controller::_bindings[ControlActionCode::ACTIONS_TOTAL_COUNT];
int Controller::_bindingsCount = 0;
InputSource* Controller::_inputSource = NULL;
Controller::_RemoteControlType Controller::_inputSourceType = _RemoteControlType::Undefined;
char* Controller::_keyboardName = NULL;


const char* Controller::_ProfileSectionToStr(_ProfileSection section)
{
    DEBUG_ASSERT(section < _ProfileSection::Undefined, NULL);  // only compile-time defined is used
    return (const char*) Controller::_ProfileSectionStr[(int)section];
}


Controller::_ProfileSection Controller::_StrToProfileSection(const char* str)
{
    _ProfileSection result = _ProfileSection::Undefined;
    int namesCount = (int) _ProfileSection::Undefined;

    for (int i = 0; i < namesCount; i++)
        if (strcmp(str, _ProfileSectionStr[i]) == 0)
        {
            result = (_ProfileSection) i;
            break;
        }

    ASSERT(result != _ProfileSection::Undefined, str);  // run-time defined data is used
    return result;
}


const char* Controller::_RemoteControlTypeToStr(_RemoteControlType rc)
{
    DEBUG_ASSERT(rc < _RemoteControlType::Undefined, NULL);  // only compile-time defined is used
    return (const char*) Controller::_RemoteControlTypeStr[(int)rc];
}


Controller::_RemoteControlType Controller::_StrToRemoteControlType(const char* str)
{
    _RemoteControlType result = _RemoteControlType::Undefined;
    int namesCount = (int) _RemoteControlType::Undefined;

    for (int i = 0; i < namesCount; i++)
        if (strcmp(str, _RemoteControlTypeStr[i]) == 0)
        {
            result = (_RemoteControlType) i;
            break;
        }

    ASSERT(result != _RemoteControlType::Undefined, str);  // run-time defined data is used
    return result;
}


void Controller::_AddControlBinding(ControlActionCode action, uint16_t key, uint8_t mods)
{
    //TODO: optimize me
    Controller::_ControlBinding binding;
    binding.action = action;
    binding.key = key;
    binding.mods = mods;

    memcpy(&Controller::_bindings[Controller::_bindingsCount], &binding, sizeof(binding));
    Controller::_bindingsCount++;
}


void Controller::_ParseLine(char* line, _ProfileSection* currentSection, _BindingName* bindingNames)
{
    char err_msg[256];
    sprintf(err_msg, "Malformed profile. Line: %s", line);

    if (line[0] == '[')  // if this line is a section name
    {
        char sectionName[16];
        memset(sectionName, '\0', sizeof(sectionName));
        memcpy(sectionName, &line[1], strlen(line) - 2);  // copy without '[' and "]\0"
        ParsingUtils::ToUpperCase(sectionName, strlen(sectionName));
        *currentSection = Controller::_StrToProfileSection(sectionName);
    }
    else
    {
        // this line must be within some section
        ASSERT(*currentSection != _ProfileSection::Undefined, err_msg);

        char *arg, *val;
        ASSERT(ParsingUtils::StrStripInPlace(line, '=', &arg, &val) == true, err_msg);
        
        ParsingUtils::StrSplitInPlaceResult splitRslt;
        memset(&splitRslt, 0, sizeof(splitRslt));
        ParsingUtils::StrSplitInPlace(val, '+', &splitRslt);

        if (*currentSection == _ProfileSection::IOSetup)
        {
            ParsingUtils::ToUpperCase(arg, strlen(arg));
            if (strcmp(arg, "SOURCE") == 0)
            {
                ParsingUtils::ToUpperCase(val, strlen(val));
                Controller::_inputSourceType = Controller::_StrToRemoteControlType(val);
                switch (Controller::_inputSourceType)
                {
                case _RemoteControlType::Keyboard:
                    Controller::_inputSource = (InputSource*) new Keyboard();
                    break;
                default:
                    ASSERT(false, err_msg);
                }
            }
            else if (strcmp(arg, "KBD_NAME") == 0)
            {
                Controller::_keyboardName = (char*) new char[strlen(val) + 1];  // +1 for '\0'
                strcpy(Controller::_keyboardName, val);
            }
            else if (strcmp(arg, "LOG_LVL") == 0)
            {
                Logger::LogLevel level = (Logger::LogLevel) ParsingUtils::StrToInt(val);
                if (level < Logger::LogLevel::Undefined)
                    Logger::SetLogLevel(level);
                else
                    Logger::PutError(ERR_LOG_SETLEVEL, Logger::LogLevel::Warning);
            }
            else
                ASSERT(false, err_msg);
        }
        else if (*currentSection == _ProfileSection::Behavior)
        {
            //TODO: implement me
        }
        else if (*currentSection == _ProfileSection::Keyboard)
        {
            uint16_t key = 0;
            uint8_t key_modifiers = 0;
            ASSERT(splitRslt.count > 0, err_msg);

            // convert the binding name (pointed by arg) to an action code
            ControlActionCode action = ControlActionCode::ACTION_INVALID;
            for (int i = 0; i < (int) ControlActionCode::ACTIONS_TOTAL_COUNT; i++)
                if (strcmp(bindingNames[i], arg) == 0)
                {
                    action = (ControlActionCode) i;
                    break;
                }
            ASSERT(action != ControlActionCode::ACTION_INVALID, err_msg);

            // we don't check double-key, double-ALT and other boring corner cases
            for (int i = 0; i < splitRslt.count; i++)
            {
                int sub_len = strlen(splitRslt.substrs[i]);
                ParsingUtils::ToUpperCase(splitRslt.substrs[i], sub_len);
                if (strcmp(splitRslt.substrs[i], "SHIFT") == 0)
                    key_modifiers |= Controller::_KEY_MODIFIER_SHIFT;
                else if (strcmp(splitRslt.substrs[i], "CTRL") == 0)
                    key_modifiers |= Controller::_KEY_MODIFIER_CTRL;
                else if (strcmp(splitRslt.substrs[i], "ALT") == 0)
                    key_modifiers |= Controller::_KEY_MODIFIER_ALT;
                else
                {
                    ASSERT(ParsingUtils::ValidateStrHex(splitRslt.substrs[i], sub_len), err_msg);
                    key = ParsingUtils::StrToInt(splitRslt.substrs[i]);
                }
            }

            Controller::_AddControlBinding(action, key, key_modifiers);
        }
#if BUILD_MODE == BUILD_DEBUG
        else
            DEBUG_ASSERT(false, "Unknown value of *currentSection.");
#endif  // BUILD_MODE == BUILD_DEBUG
    }
}


void Controller::_InitInputSource()
{
    Err initResult = ERR_OK;
    if (Controller::_inputSourceType == _RemoteControlType::Keyboard)
    {
        KeyboardInitArgs args = {Controller::_keyboardName, Controller::__FckConvert};
        initResult = Controller::_inputSource->Init(&args);
    }

    if (initResult != ERR_OK)
    {
        char loggerMsg[256];
        sprintf(loggerMsg, "Controller: unable to init input src %d, error %X", (int)Controller::_inputSourceType, initResult);
        Logger::Put(loggerMsg, Logger::LogLevel::HwFailure);
    }
}


//TODO: optimize it and remove away ASAP !!!
uint16_t Controller::__FckConvert(KeyboardEvent* evt)
{
    uint8_t mods = 0;
    if (evt->shift)
        mods |= Controller::_KEY_MODIFIER_SHIFT;
    if (evt->ctrl)
        mods |= Controller::_KEY_MODIFIER_CTRL;
    if (evt->alt)
        mods |= Controller::_KEY_MODIFIER_ALT;

    for (int i = 0; i < Controller::_bindingsCount; i++)
        if (Controller::_bindings[i].key == evt->key && Controller::_bindings[i].mods == mods)
        {
            return Controller::_bindings[i].action;
        }

    return (uint16_t) ControlActionCode::ACTION_INVALID;
}


bool Controller::Initialize(char* binding_data, char* profile_data)
{
    char* data_line = NULL;
    _ProfileSection currentSection = _ProfileSection::Undefined;

    // build a binding names list
    Controller::_BindingName bindingNames[ControlActionCode::ACTIONS_TOTAL_COUNT];
    int bindingNamesCnt = 0;
    while ( (data_line = ParsingUtils::GetNextLine(binding_data)) != NULL )
    {
        binding_data = &binding_data[strlen(data_line) + 1];  // +1 means include a '\n'

        char* bindingName = ParsingUtils::RemoveComment(data_line);
        int bindingNameLen = strlen(bindingName);
        if (bindingNameLen > 0)
        {
            if (bindingNameLen > _BindingName_MaxLength)
            {
                char msg[256] = "Binding name is too long: ";  // although an input file with binding names is a static piece of data, it was
                ASSERT(false, strcat(msg, bindingName));       // decided to keep this assertion in the release mode.
            }
            strcpy(bindingNames[bindingNamesCnt], bindingName);
            bindingNamesCnt++;
        }

        delete[] data_line;
    }

    // parse the profile
    while ( (data_line = ParsingUtils::GetNextLine(profile_data)) != NULL )
    {
        profile_data = &profile_data[strlen(data_line) + 1];  // +1 means include a '\n'

        char* lexem = ParsingUtils::RemoveComment(data_line);
        if (strlen(lexem) > 0)
        {
            Controller::_ParseLine(lexem, &currentSection, bindingNames);
        }

        delete[] data_line;
    }

    Controller::_InitInputSource();
    return true;
}


void Controller::TakeControl()
{
    InputEvent evt;
    bool proceedListening = true;
    char loggerMsg[256];
    memset(loggerMsg, 0, sizeof(loggerMsg));

    Transmission* transmsn = (Transmission*) DeviceManager::GetDevice((char*)"transmsn");
    DEBUG_ASSERT(transmsn != NULL, "No transmission");

    while (proceedListening) 
    {
        evt.dw = 0;
        Controller::_inputSource->WaitInputEvent(&evt);
        
        switch (evt.bt.action)
        {
        case ControlActionCode::ACTION_ExitApp:
            proceedListening = false;
            break;
        case ControlActionCode::ACTION_LeftTrackForward:
            if (evt.bt.begin)
                transmsn->LeftTrack(Transmission::Direction::Forward);
            else
                transmsn->LeftTrack(Transmission::Direction::Neutral);
            break;
        case ControlActionCode::ACTION_LeftTrackBackward:
            if (evt.bt.begin)
                transmsn->LeftTrack(Transmission::Direction::Backward);
            else
                transmsn->LeftTrack(Transmission::Direction::Neutral);
            break;
        case ControlActionCode::ACTION_RightTrackForward:
            if (evt.bt.begin)
                transmsn->RightTrack(Transmission::Direction::Forward);
            else
                transmsn->RightTrack(Transmission::Direction::Neutral);
            break;
        case ControlActionCode::ACTION_RightTrackBackward:
            if (evt.bt.begin)
                transmsn->RightTrack(Transmission::Direction::Backward);
            else
                transmsn->RightTrack(Transmission::Direction::Neutral);
            break;
        default:
            sprintf(loggerMsg, "Unknown action code received (%x)", evt.bt.action);
            Logger::Put(loggerMsg, Logger::LogLevel::Warning);
            break;
        }
    }
}


void Controller::Destroy()
{
    if (Controller::_inputSource != NULL)
    {
        delete Controller::_inputSource;
        Controller::_inputSource = NULL;
    }

    if (Controller::_keyboardName != NULL)
    {
        delete [] Controller::_keyboardName;
        Controller::_keyboardName = NULL;
    }
}
