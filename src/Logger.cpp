#include <stdio.h>
#include <string.h>
#include "ProjectCfg.h"

#if BUILD_PLATFORM == BUILD_LINUX
#   include <sys/types.h>
#   include <unistd.h>
#endif  // BUILD_PLATFORM == BUILD_LINUX

#include "Logger.h"


bool Logger::_isStd = false;
bool Logger::_isFile = false;
bool Logger::_isPipe = false;
char* Logger::_logFilePath = NULL;

char Logger::_buffer[] = {0};
int Logger::_bytesInBuffer = 0;
Logger::LogLevel Logger::_lastLvl = Logger::LogLevel::Info;
Logger::LogLevel Logger::_thresholdLvl = Logger::LogLevel::Debug;


void Logger::_Flush(char* buf, LogLevel lvl)
{
    if (Logger::_isStd)
        printf("[%d] %s\n", lvl, buf);

    if (Logger::_isFile)
    {
        FILE* hFile = fopen(Logger::_logFilePath, "a");
        if (hFile != NULL)
        {
            fprintf(hFile, "[%d] %s\n", lvl, buf);
            fclose(hFile);
        }
        else  // Real shit happened, and we can't hang in assert (it can cause Logger also).
        {     // So let's try the last way to inform RC-op -- use a pipe
            //TODO: use pipe to write log / blink
        }
    }

    if (Logger::_isPipe)
    {}  //TODO: use pipe

    if ((void*)buf == (void*)Logger::_buffer)  // oh, it's my own buffer actually!
    {                                          // I have to clean it up!
        Logger::_bytesInBuffer = 0;
        memset(Logger::_buffer, 0, Logger::_BUFFER_SIZE);
    }
}


void Logger::AllowStdoutLog(bool on)
{
    Logger::_isStd = on;
}


void Logger::AllowFileLog(bool on, char* log_path)
{
    Logger::_isFile = on;
    if (on)  // enable
        Logger::_logFilePath = log_path;
    else  // disable
        Logger::_logFilePath = NULL;
}


void Logger::AllowPipeLog(bool on)
{
    //TODO:
    /*
    Logger::_isPipe = on;
    if (on)  // enable
    {
#if BUILD_PLATFORM == BUILD_LINUX
       if (pipe(Logger::_pipe))  // bad case
       {
           fprintf (stderr, "Pipe failed.\n");
           return;  //TODO: return EXIT_FAILURE;
       }
       else  // good case
           Logger::_isPipe = on;
#elif BUILD_PLATFORM == BUILD_WINDOWS

#endif  // BUILD_PLATFORM == BUILD_LINUX
    }
    else  // disable
    {
    
    }
    */
}


void Logger::Put(char* txt, LogLevel lvl)
{
    if (lvl < Logger::_thresholdLvl)
        return;

    if (Logger::_bytesInBuffer > 0)
        Logger::_Flush(Logger::_buffer, lvl);
    Logger::_Flush(txt, lvl);
}


void Logger::PutError(Err err, LogLevel lvl)
{
    if (lvl < Logger::_thresholdLvl)
        return;

    char loggerMsg[256];
    sprintf(loggerMsg, "Error happened: %X", err);
    Logger::Put(loggerMsg, lvl);
}


void Logger::Mem(char* txt, LogLevel lvl, bool force_flush)
{
    if (lvl < Logger::_thresholdLvl)
        return;

    int txtLen = strlen(txt);
    if (txtLen > Logger::_BUFFER_SIZE)  // if it is too long there is no sense
    {                                   // to keep anything of txt in _buffer
        Logger::Put(txt, lvl);
    }
    else  // good case: can work as designed
    {
        int bytesLeft = txtLen;
        while (bytesLeft > 0)
        {
            int bytesToSend = Logger::_BUFFER_SIZE - Logger::_bytesInBuffer > bytesLeft ?
                    bytesLeft : Logger::_BUFFER_SIZE - Logger::_bytesInBuffer;
            memcpy(&Logger::_buffer[Logger::_bytesInBuffer], &txt[txtLen - bytesLeft], bytesToSend);
            Logger::_bytesInBuffer += bytesToSend;
            bytesLeft -= bytesToSend;

            if (Logger::_bytesInBuffer == Logger::_BUFFER_SIZE)
                Logger::_Flush(Logger::_buffer, LogLevel::Info);
        }

        if (force_flush && Logger::_bytesInBuffer > 0)
            Logger::_Flush(Logger::_buffer, lvl);
        else
            Logger::_lastLvl = lvl;
    }
}


void Logger::Flush()
{
    if (Logger::_bytesInBuffer > 0)
        Logger::_Flush(Logger::_buffer, Logger::_lastLvl);    
}


void Logger::DefaultSetup(bool mute)
{
    if (mute == true)
    {
        Logger::AllowStdoutLog(false);
        Logger::AllowFileLog(false, NULL);
        Logger::AllowPipeLog(false);
    }
    else
    {
#if BUILD_MODE == BUILD_DEBUG
        Logger::AllowStdoutLog(true);
#elif BUILD_MODE == BUILD_ALPHA
        Logger::AllowStdoutLog(true);
        Logger::AllowFileLog(true, "/home/pi/rctank-default.log");
#elif BUILD_MODE == BUILD_RELEASE
        Logger::AllowFileLog(true, "/home/pi/rctank-default.log");
#endif  // BUILD_MODE
    }
}


void Logger::SetLogLevel(LogLevel lvl)
{
    char msg[256];
    sprintf(msg, "Setting Log threshold: %d", (int) lvl);
    Logger::Put(msg, LogLevel::Info);
    Logger::_thresholdLvl = lvl;
}


Logger::LogLevel Logger::GetLogLevel()
{
    return Logger::_thresholdLvl;
}
