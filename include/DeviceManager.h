#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include "RPi.h"
#include "GpioDevice.h"


class DeviceManager
{
private:
    static const int _DEVICE_INFO_NAME_MAXLEN = 15;
    static const int _DEVICE_INFO_TYPE_MAXLEN = 15;
    typedef struct
    {
        char   name[_DEVICE_INFO_NAME_MAXLEN+1];
        char   type[_DEVICE_INFO_TYPE_MAXLEN+1];
        uint8_t  pins[RPI_PIN_COUNT];  //TODO: optimize to use bit-map instead of array
        int    pinCnt;
    } _DeviceInfo;

    typedef struct _hidden_DeviceNode
    {
        GpioDevice* device;
        _DeviceInfo device_info;
        _hidden_DeviceNode* next;
    } _DeviceNode;

    static _DeviceNode* _devices;

    // Converts input text into a parsed info if possible.
    // @arg 'line':
    //     pointer to a buffer with a complete text line from a config-file. The text will be changed
    //     during execution, so make sure you have original copy if need.
    // @arg [out] 'device_info':
    //     parsed info which can be used for registering a new device.
    static void _ParseLine(char* line, _DeviceInfo* device_info);

    // Dynamically extends the '_devices' list creating an object of type specified in 'device_info'.
    // The list of objects and list itself must be destructed. Also it makes a check for correct
    // pins usage.
    static void _RegisterDevice(_DeviceInfo* device_info, DeviceID dev_id, int* mutex_id);

public:
    static bool Initialize(char* cfg_file_data);
    static GpioDevice* GetDevice(char* name);

    // A static destructor. It frees all allocated heap memory, pulls to 0 all used GPIOs and
    // releases them.
    static void Destroy();
};

#endif  // __DEVICE_MANAGER_H__
