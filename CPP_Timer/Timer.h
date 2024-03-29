///////////////////////////////////////////////////////////////////////////////
//!
//! @file		Timer.h
//! 
//! @brief		A singleton class to handle timer functionality 
//! 
//! @author		Chip Brommer
//! 
//! @date		< 1 / 12 / 2022 > Initial Start Date
//!
/*****************************************************************************/
#pragma once
///////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#pragma comment(lib, "Winmm.lib")		
#ifdef _WIN32
#include <iostream>						// IO stream
#include <windows.h>					// Windows 
#else
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif
//
#include <stdint.h>						// Standard integer types
#include <string>						// Strings
#include <thread>						// Threading
#include "../Log.h"						// Logging
//
// 
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CPP_TIMER					// Define the cpp timer class. 
#define     CPP_TIMER
//
#ifndef		CPP_LOGGER					// If CPP_Logger not included, use normal IO
#define		USE_STDIO
#endif
//
///////////////////////////////////////////////////////////////////////////////
namespace Essentials
{
	class Timer
	{
	public:
		//! @brief Prevent cloning.
		Timer(Timer& other) = delete;

		//! @brief Prevent assigning
		void operator=(const Timer&) = delete;

		//! @brief Get current instance or creates a new one. 
		static Timer* GetInstance();

		//! @brief Release the instance
		static void		ReleaseInstance();

		//! @brief Resets the timer
		void			Reset();

		//! @brief Get the current timer ticks in Milliseconds
		uint32_t		GetMSecTicks();

		//! @brief Get the current timer ticks in Microseconds
		uint32_t		GetUSecTicks();

		//! @brief Milliseconds sleep command
		void			MSecSleep(const uint32_t mSecs);

		//! @brief Microseconds sleep command
		void			USecSleep(const uint32_t uSecs);

	protected:
	private:
		//<! FUNCTIONS
		Timer();					//!< Hidden Constructor
		~Timer();					//!< Hidden Deconstructor

		//! @brief Hidden Initializer - starts the thread. 
		void			Initialize();

		//! @brief Handle True Milliseconds as an event
		int 			HandleTrueMSec();

		//! @brief Error output
		void			Fatal(std::string msg);

		//!< VARIABLES
		bool			mInitialzied;				// Track if initialized
		bool			mClosing;					// Track if closing.
		uint32_t		mTickOffset;				// Timer offset from start up timer
		bool			mTimerThreadReady;			// Thread flag
		uint64_t		mUSecStartTime;				// System start time in usec
		volatile uint32_t	mTickCount;				// Tick count
		double			mTimerFactor;				// Timer factor
		static Timer*	mInstance;					// Instance of Logger
		std::thread*	mThread;					// Pointer to a thread object
		std::string		mUser;						// System User for Log information location 
	};
}
#endif // CPP_TIMER