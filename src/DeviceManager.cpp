#include <string.h>
#include <wiringPi.h>

#include "Asserts.h"
#include "Logger.h"
#include "ParsingUtils.h"
#include "LED.h"
#include "RGBLED.h"
#include "RaspberryPiCamera.h"
#include "Transmission.h"
#include "DeviceManager.h"


DeviceManager::_DeviceNode* DeviceManager::_devices = NULL;


void DeviceManager::_ParseLine(char* line, _DeviceInfo* device_info)
{
    char *arg, *val;

    // here we have: "my_dev#1 = RGB-LED ( 1, 2, 3 )"  // [arg = val]
    // turns into  : "my_dev#1`RGB-LED ( 1, 2, 3 )"    // '`' means '\0' 
    ASSERT(ParsingUtils::StrStripInPlace(line, '=', &arg, &val) == true, line);
    
    // if StrStripInPlace(val) will return 'true', cfg-file is malformed: multiple '='
    DEBUG_ASSERT(ParsingUtils::StrStripInPlace(val, '=', &arg, &val) == false, line);
    DEBUG_ASSERT(strlen(arg) <= DeviceManager::_DEVICE_INFO_NAME_MAXLEN, "Dev name is too long.");
    strcpy(device_info->name, arg);

    // here we have: "RGB-LED ( 1, 2, 3 )"  // [type and val]
    // turns into  : "RGB-LED` 1, 2, 3 )"
    char* delimPtr = strchr(val, '(');
    DEBUG_ASSERT(delimPtr != NULL, val);  // if false, cfg-file is malformed
    DEBUG_ASSERT(strchr(delimPtr+1, '(') == NULL, val);  // if false, cfg-file is malformed: multiple '('
    *delimPtr = '\0';
    char* type = ParsingUtils::TrimString(val);
    DEBUG_ASSERT(strlen(type) <= DeviceManager::_DEVICE_INFO_TYPE_MAXLEN, "Dev type is too long.");
    strcpy(device_info->type, type);

    // here we have: " 1, 2, 3 )"  // [val]
    // turns into  : "1, 2, 3`"
    val = delimPtr + 1;
    delimPtr = strchr(val, ')');
    *delimPtr = '\0';
    val = ParsingUtils::TrimString(val);
    
    ParsingUtils::ToUpperCase(val, strlen(val));

    // split the list in-place into set of strings
    char* strings[RPI_PIN_COUNT];
    int stringCnt = 0;
    int val_charCount = strlen(val);
    char* string_start = val;
    for (int i = 0; i < val_charCount; i++)
    {
        if (val[i] == ',')
        {
            strings[stringCnt] = string_start;
            stringCnt++;
            string_start = &val[i+1];
            val[i] = '\0';
        }
    }
    strings[stringCnt++] = string_start;  // add the last string since it doesn't has trailing ','
    DEBUG_ASSERT(stringCnt <= RPI_PIN_COUNT, "Buffer \"strings\" overflow.");

    // extract raw pin numbers or GPIO numbers (WiringPi numbering) or interface name
    // from the strings
    int pinsIssued = 0;
    for (int i = 0; i < stringCnt; i++)
    {
        char* separator;
        bool isGpio = false;
        int shift = 0;  // how many chars we have to omit from the beginning of strings[i]
                        // for skipping a token if any (e.g. "GPIO-")
        strings[i] = ParsingUtils::TrimString(strings[i]);
        if ( (separator = strchr(strings[i], '-')) != NULL    // if '-' is used
                || (separator = strchr(strings[i], '_')) != NULL)  // of if '_' is used
            shift = 1;

        if (strcmp(strings[i], "I2C") == 0)
        {
            device_info->pins[pinsIssued] = (uint8_t) 3;
            device_info->pins[pinsIssued+1] = (uint8_t) 5;
            pinsIssued += 2;
            continue;
        }
        else if (strcmp(strings[i], "UART") == 0)
        {
            device_info->pins[pinsIssued] = (uint8_t) 8;
            device_info->pins[pinsIssued+1] = (uint8_t) 10;
            pinsIssued += 2;
            continue;
        }
        else if (strcmp(strings[i], "SPI") == 0)
        {
            device_info->pins[pinsIssued] = (uint8_t) 19;
            device_info->pins[pinsIssued+1] = (uint8_t) 21;
            device_info->pins[pinsIssued+2] = (uint8_t) 23;
            device_info->pins[pinsIssued+3] = (uint8_t) 24;
            device_info->pins[pinsIssued+4] = (uint8_t) 26;
            pinsIssued += 5;
            continue;
        }
        else if (strstr(strings[i], "GPIO") == strings[i])
        {
            shift += 4;  // += strlen("GPIO")
            isGpio = true;
        }
        else if (strstr(strings[i], "PIN") == strings[i])
            shift += 3;  // += strlen("PIN")
        
        DEBUG_ASSERT(ParsingUtils::ValidateStrDigits(&strings[i][shift], strlen(&strings[i][shift])), strings[i]);
        
        // validate pin/GPIO, convert pin to GPIO if need
        uint32_t number = ParsingUtils::StrToInt(&strings[i][shift]);//(uint32_t) atoi(&strings[i][shift]);
        if (isGpio)  // 'number' is GPIO number
        {
            DEBUG_ASSERT(RPi_GpioExist(number) == RPI_TRUE, strings[i]);
            device_info->pins[pinsIssued++] = (uint8_t) RPi_GpioToPin(number);
        }
        else  // 'number' is raw pin number
        {
            DEBUG_ASSERT(number <= RPI_PIN_COUNT, strings[i]);
            device_info->pins[pinsIssued++] = (uint8_t) number;
        }
    }
    device_info->pinCnt = pinsIssued;
}


void DeviceManager::_RegisterDevice(_DeviceInfo* device_info, DeviceID dev_id, int* mutex_id)
{
    // Check for conflicts: one GPIO cannot be used by multiple devices (however, Ground and
    // Power pins can). It is a purely debug code, so make sure all connections are OK using
    // BUILD_ALPHA or BUILD_DEBUG mode.
#if BUILD_MODE < BUILD_RELEASE
    _DeviceNode* node = DeviceManager::_devices;
    while (node != NULL)
    {
        for (int i = 0; i < device_info->pinCnt; i++)
        {
            if (RPi_Pin_Map[ device_info->pins[i] ].gpio == RPI_NOT_GPIO)  // Skip Ground and Power
                continue;                                                  // pins in this check.

            for (int j = 0; j < node->device_info.pinCnt; j++)
            {
                DEBUG_ASSERT(device_info->pins[i] != node->device_info.pins[j], device_info->name);
            }
        }
        node = node->next;
    }

    int specified_gpio_cnt = 0;
    for (int i = 0; i < device_info->pinCnt; i++)
        if (RPi_Pin_Map[ device_info->pins[i] ].gpio != RPI_NOT_GPIO)
            specified_gpio_cnt++;
#endif  // BUILD_MODE < BUILD_RELEASE

    // Create an appropriate object ('new_device')
    ParsingUtils::ToUpperCase(device_info->type, strlen(device_info->type));
    GpioDevice* new_device = NULL;
    if (strcmp(device_info->type, "LED") == 0)
    {
        DEBUG_ASSERT(specified_gpio_cnt == 1, device_info->name);
        new_device = (GpioDevice*) new LED(dev_id, device_info->pins[0]);
        new_device->SetMutexID(*mutex_id);
        (*mutex_id)++;
    }
    else if (strcmp(device_info->type, "RGBLED") == 0)
    {
        uint8_t* pins = device_info->pins;
        DEBUG_ASSERT(specified_gpio_cnt == 3, device_info->name);
        new_device = (GpioDevice*) new RGBLED(dev_id, pins[0], pins[1], pins[2]);
        new_device->SetMutexID(*mutex_id);
        (*mutex_id)++;
    }
    else if (strcmp(device_info->type, "RPICAM") == 0)
    {
        new_device = (GpioDevice*) new RaspberryPiCamera(dev_id, device_info->pins, device_info->pinCnt);
    }
    else if (strcmp(device_info->type, "TRANSMISSION") == 0)
    {
        uint8_t* pins = device_info->pins;
        DEBUG_ASSERT(specified_gpio_cnt == 6, device_info->name);
        new_device = (GpioDevice*) new Transmission(dev_id, pins[0], pins[1], pins[2], pins[3], pins[4], pins[5]);
    }
    else  // bad case: unknown device type
    {
        DEBUG_ASSERT(false, device_info->type);
    }

    // Add the 'new_device' to a device list
    ASSERT(new_device != NULL, NULL);
    _DeviceNode* new_node = new _DeviceNode();
    new_node->device = new_device;
    memcpy(&new_node->device_info, device_info, sizeof(_DeviceInfo));
    new_node->next = DeviceManager::_devices;
    DeviceManager::_devices = new_node;
}


bool DeviceManager::Initialize(char* cfg_file_data)
{
    char* data_line = NULL;
    DeviceID data_line_number = 0;
    int mutexID = 0;

    if (wiringPiSetup() != 0)
    {
        Logger::Put("DeviceManager: wiringPiSetup() failed.", Logger::LogLevel::FatalError);
        return false;
    }

    while ( (data_line = ParsingUtils::GetNextLine(cfg_file_data)) != NULL )
    {
        data_line_number++;
        cfg_file_data = &cfg_file_data[strlen(data_line) + 1];  // +1 means include a '\n'

        char* lexem = ParsingUtils::RemoveComment(data_line);
        if (strlen(lexem) > 0)
        {
            _DeviceInfo dev_info;
            memset(&dev_info, 0, sizeof(dev_info));
            DeviceManager::_ParseLine(lexem, &dev_info);
            DeviceManager::_RegisterDevice(&dev_info, data_line_number, &mutexID);
        }

        delete[] data_line;
    }

    return true;
}


GpioDevice* DeviceManager::GetDevice(char* name)
{
    _DeviceNode* node = DeviceManager::_devices;
    while (node != NULL)
    {
        if (strcmp(node->device_info.name, name) == 0)
            return node->device;
        node = node->next;
    }
    return NULL;
}


void DeviceManager::Destroy()
{
    while (DeviceManager::_devices != NULL)
    {
        _DeviceNode* target = DeviceManager::_devices;
        DeviceManager::_devices = DeviceManager::_devices->next;
        target->device->Switch(false);
        delete target->device;
        delete target;
    }
}
