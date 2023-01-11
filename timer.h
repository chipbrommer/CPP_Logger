#pragma once
#pragma comment(lib, "Winmm.lib")
#ifdef _WIN32
#include <stdio.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

#include "Log.h"
#include <stdint.h>

uint32_t TIMER_GetMsecTicks();
uint32_t TIMER_GetUsecTicks();
void   TIMER_MsecSleep(uint32_t milliSecs);
void   TIMER_UsecSleep(uint32_t microSecs);
void   TIMER_Reset(void);