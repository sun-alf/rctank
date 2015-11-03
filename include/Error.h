#ifndef __ERROR_H__
#define __ERROR_H__

typedef enum
{
    ERR_OK = 0,
    ERR_KBD_OPEN = 0x00000001,
    ERR_KBD_READ = 0x00000002,
    ERR_LOG_SETLEVEL = 0x00000010,
} Err;

#endif  // __ERROR_H__
