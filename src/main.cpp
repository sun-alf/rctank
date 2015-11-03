#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Asserts.h"
#include "ParsingUtils.h"
#include "Logger.h"
#include "DeviceManager.h"
#include "Controller.h"
#include "LED.h"
#include "RGBLED.h"
#include "RaspberryPiCamera.h"

#define RES_BINDING_FILE_PATH       "./res/binding.res"


static char* _GetArgValue(char* arg)
{
    char* result = strchr(arg, '=');
    return ++result;
}


int main(int argc, char *argv[])
{
    int result = -1;  // must be set to 0 when reaching end-of-program successfully
    char* binding_data = NULL;
    char* cfg_file_data = NULL, *profile_data = NULL;
    char* cfg_file_path = NULL, *profile_path = NULL;

    bool issueLogPipe = false;

    // default Logger set-up. It can be re-defined via command line
    bool loggerDefaultSetup = true;
    Logger::DefaultSetup(false);

#if BUILD_MODE == BUILD_DEBUG
    if (argc < 2)
    {  // emulate cmd line args
        char* fake_argv[] = {
            "me",
            "--cfg=res/gpiodev.cfg",
            "--profile=res/profile_win.cfg",
            "--log=mylog.log",
            "--logger=std"
        };
        argv = fake_argv;
        argc = 3;
    }
#endif  // BUILD_MODE == BUILD_DEBUG

    for (int arg = 1; arg < argc; arg++)
    {
        // if strstr() returns an address of the argv[x] beginning
        // then that's an arg we are checking for
        if (strstr(argv[arg], "--cfg=") == argv[arg])
        {
            cfg_file_path = _GetArgValue(argv[arg]);
            DEBUG_ASSERT(strlen(cfg_file_path) > 0, "--cfg path is broken");
        }
        else if (strstr(argv[arg], "--profile=") == argv[arg])
        {
            profile_path = _GetArgValue(argv[arg]);
            DEBUG_ASSERT(strlen(profile_path) > 0, "--profile path is broken");
        }
        else if (strstr(argv[arg], "--assert_gpio=") == argv[arg])
        {
            char* str_number = _GetArgValue(argv[arg]);
            SetAssertionLedGpio(atoi(str_number));
        }
        else if (strstr(argv[arg], "--log=") == argv[arg])
        {
            char* fpath = _GetArgValue(argv[arg]);
#if BUILD_MODE < BUILD_RELEASE
            FILE* f = fopen(fpath, "a");
            DEBUG_ASSERT(f != NULL, "--log path is broken or file is not accessible for writing");
#endif  // BUILD_MODE < BUILD_RELEASE
            if (loggerDefaultSetup)
            {
                Logger::DefaultSetup(true);
                loggerDefaultSetup = false;
            }
            Logger::AllowFileLog(true, fpath);
        }
        else if (strstr(argv[arg], "--logger=") == argv[arg])
        {
            char* val = _GetArgValue(argv[arg]);

            if (loggerDefaultSetup)
            {
                Logger::DefaultSetup(true);
                loggerDefaultSetup = false;
            }

            if (strstr(val, "std") != NULL)
                Logger::AllowStdoutLog(true);

#if BUILD_PLATFORM == BUILD_LINUX
            if (strstr(val, "pipe") != NULL)
            {
                Logger::AllowPipeLog(true);
                issueLogPipe = true;
            }
#endif  // BUILD_PLATFORM == BUILD_LINUX
        }
        else
        {
            char msg[256] = "Unknown arg: ";
            Logger::Put(strcat(msg, argv[arg]), Logger::LogLevel::Warning);
        }
    }

    // cmd-line args are parsed, it is time to fork() if need
    if (issueLogPipe == true)
    {
        //TODO: do a fork() in order to support piping
    }

    // Load config files
    DEBUG_ASSERT(cfg_file_path != NULL, "--cfg= is a mandatory arg.");
    cfg_file_data = ParsingUtils::LoadFile(cfg_file_path);
    DEBUG_ASSERT(profile_path != NULL, "--profile= is a mandatory arg.");
    profile_data = ParsingUtils::LoadFile(profile_path);
    binding_data = ParsingUtils::LoadFile(RES_BINDING_FILE_PATH);

    do  // open a breakable code construction
    {
        // Initialize GPIOs and GPIO devices
        if (DeviceManager::Initialize(cfg_file_data) == false) break;
        if (Controller::Initialize(binding_data, profile_data) == false) break;

        delete [] cfg_file_data;
        delete [] profile_data;
        delete [] binding_data;

        Controller::TakeControl();

        Controller::Destroy();
        DeviceManager::Destroy();
        result = 0;
    } while (0);

    return result;
}
