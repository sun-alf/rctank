#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "ProjectCfg.h"
#include "Error.h"


class Logger
{
public:
    enum class LogLevel
    {
        Debug = 0, Info, Warning, HwFailure, Assert, FatalError, Undefined
    };

private:
    static const int _BUFFER_SIZE = 1024;

    static bool _isStd;
    static bool _isFile;
    static bool _isPipe;
    static char* _logFilePath;

    static char _buffer[_BUFFER_SIZE+1];
    static int _bytesInBuffer;
    static LogLevel _lastLvl;
    static LogLevel _thresholdLvl;

#if BUILD_PLATFORM == BUILD_LINUX
    static int _pipe[2];
#elif BUILD_PLATFORM == BUILD_WINDOWS

#endif  // BUILD_PLATFORM == BUILD_LINUX

    static void _Flush(char* buf, LogLevel lvl);

public:
    static void AllowStdoutLog(bool on);

    // Doesn't perform a copying of "log_path" string to a internal buffer, only
    // a pointer to the "log_path" is saved. So don't lose the path you are providing.
    static void AllowFileLog(bool on, char* log_path);

    static void AllowPipeLog(bool on);

    // It opens, writes and closes log file (if allowed) each time it is called, so don't use
    // it too often -- this may affect total performance and microSD's medium. It puts the
    // 'txt' buffer immediately into allowed stdout/pipe also.
    static void Put(char* txt, LogLevel lvl);
    static void PutError(Err err, LogLevel lvl);

    // Does the same things as ::Put() but actual flush to allowed destinations is delayed
    // until internal buffer is full. This method keeps in mind any message for some time,
    // so performance is better but there is a risk to lose log data in case of unhandled
    // failure. Use force_flush=true to accomplish flushing immediately.
    static void Mem(char* txt, LogLevel lvl, bool force_flush);

    static void Flush();

    // Allows default output streams (stdout|file|pipe). It depends on BUILD_MODE!
    // Or disallows all output streams if 'mute' is 'true'.
    static void DefaultSetup(bool mute);

    // Set/get logging threshold. Logger won't put entries with level below this one.
    static void SetLogLevel(LogLevel lvl);
    static LogLevel GetLogLevel();
};

#endif  // __LOGGER_H__
