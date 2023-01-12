///////////////////////////////////////////////////////////////////////////////
//!
//! @file		timer.h
//! 
//! @brief		A non-class timer.
//! 
//! @author		Chip Brommer
//! 
//! @date		< 12 / 15 / 2022 > Initial Start Date
//!
/*****************************************************************************/
#pragma once
///////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#ifdef _WIN32
#pragma comment(lib, "Winmm.lib")		// Multimedia library	
#include <iostream>						// IO
#include <windows.h>					// Windows library
#else
#include <sys/time.h>					// System time
#include <time.h>						// Time
#include <unistd.h>						// Constants and types	
#endif
//
#include "Log.h"						// Logging class
#include <stdint.h>						// Standard integer types
//
// 
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#if !defined OLD_TIMER					// Define the timer
#define OLD_TIMER
#endif
//
///////////////////////////////////////////////////////////////////////////////

uint32_t TIMER_GetMsecTicks();
uint32_t TIMER_GetUsecTicks();
void   TIMER_MsecSleep(uint32_t milliSecs);
void   TIMER_UsecSleep(uint32_t microSecs);
void   TIMER_Reset(void);