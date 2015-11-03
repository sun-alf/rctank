#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <windows.h>

#include "wiringPi.h"

#define _BEGIN_THREAD_ERROR  ((unsigned long)-1L)


typedef void (*ThreadFuncPtr)(void* args);

static HANDLE piMutexes[PI_THREAD_MUTEX_COUNT];


static void _piThreadingExit()
{
    for (int i = 0; i < PI_THREAD_MUTEX_COUNT; i++)
    {
        WaitForSingleObject(piMutexes[i], INFINITE);
        ReleaseMutex(piMutexes[i]);
    }
}


int piThreadingInit()
{
    for (int i = 0; i < PI_THREAD_MUTEX_COUNT; i++)
    {
        char mutexName [16];
        sprintf(mutexName, "piMutex_%d", i);
        piMutexes[i] = CreateMutex(NULL, FALSE, mutexName);
        if (piMutexes[i] == NULL)  // bad case
            return -1;
    }
    atexit(_piThreadingExit);
    return 0;
}


int piThreadCreate (void *(*fn)(void *), void* args)
{
    ThreadFuncPtr my_fn_ptr = (ThreadFuncPtr) fn;
    if (_beginthread(my_fn_ptr, 0, args) != _BEGIN_THREAD_ERROR)  // good case
        return 0;
    else  // bad case
        return 100500;
}


void piLock (int key)
{
    if (key < PI_THREAD_MUTEX_COUNT)
        WaitForSingleObject(piMutexes[key], INFINITE);
    else  // bad case
        printf("piLock(): bad key [%d]", key);
}


void piUnlock (int key)
{
    if (key < PI_THREAD_MUTEX_COUNT)
        ReleaseMutex(piMutexes[key]);
    else  // bad case
        printf("piUnlock(): bad key [%d]", key);
}


void delay (unsigned int howLong)
{
    Sleep(howLong);
}
