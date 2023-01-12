///////////////////////////////////////////////////////////////////////////////
//!
//! @file		Log.h
//! 
//! @brief		A singleton class to handle asyncronous logging to a file of 
//!				various levels of importance. 
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
#if defined _WIN32
#include	<windows.h>					// Windows necessary stuff
#include	<direct.h>					// Make Directory
#else
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#endif
//
#include	<string>                    // Strings
#include	<fstream>					// File Stream
#include	<iostream>					// Input Output
#include	<thread>					// Multithreading
#include	<queue>						// Queue object to store pending log entries
#include	<mutex>						// Mutex object to enable thread safe usage of the queue
#include	<chrono>					// Timing for filename date/time
#include	<cstring>					// C-Strings
#include	<stdarg.h>					// Inbound Arguments
#include	<debugapi.h>				// Debug Message
//
#include	"timer.h"					// Old non-class timer
#include	"LogInfo.h"					// Program Info
//
// 
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CPP_LOGGER					// Define the cpp logger class. 
#define     CPP_LOGGER
#endif
//
#if !defined (CPP_TIMER) && !defined (OLD_TIMER)
#define		NO_TIMER
#endif
//
///////////////////////////////////////////////////////////////////////////////

// Levels of logging
enum {
	LOG_NONE = 0,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG
};

// Time stamp options
enum {
	LOG_TS_NONE,
	LOG_TS_MSEC,
	LOG_TS_USEC
};

class Log
{
public:
	//! @brief Prevent cloning.
	Log(Log& other) = delete;

	//! @brief Prevent assigning
	void operator=(const Log&) = delete;

	//! @brief Get current instance or creates a new one. 
	static Log* GetInstance();

	//! @brief Release the instance
	static void ReleaseInstance();

	//! @brief Starts the logging thread
	//! @param filename - File name and path to place the output file
	//! @param enableConsoleLogging - true by default, enables or disables console logging. 
	//! @param enableFileLogging - true by default, enables or disables file logging.
	//! @return -1 on fail, 0 if already initialized, 1 if successful
	int	Initialize(std::string filename, bool enableConsoleLogging = true, bool enableFileLogging = true);

	//! @brief Adds a message into the queue to be logged
	//! @param level - LOG level of the string.
	//! @param user - User the message is coming from
	//! @param format - formatted string to be logged. 
	//! @return false if failed, true if message was logged
	bool	AddEntry(int level, std::string user, std::string format, ...);

	//! @brief Writes out the log entries. 
	void	WriteOut();

	//! @brief Sets the maximum logging level.
	//! @param level - Maximum level to be logged to console and file.
	//! @return false if failed, true if set
	bool    SetLogLevel(int level);

	//! @brief Sets the timestamp logging type
	//! @param tsLevel - Maximum timestamp level to be logged to console and file.
	//! @return false if failed, true if set
	bool    SetLogTimestampLevel(int tsLevel);

	//! @brief Turn on/off logging to console.
	//! @param enabled - enable logging to console ?
	//! @return false if failed, true if set
	bool	LogToConsole(bool enable);

	//! @brief Turn on/off logging to file.
	//! @param enabled - enable logging to file ?
	//! @return false if failed, true if set
	bool	LogToFile(bool enable);

protected:
private:
	Log();															//!< Hidden Constructor
	~Log();															//!< Hidden Deconstructor
	static Log* mInstance;											//!< Instance of Logger
	std::thread* mThread;											//!< Pointer to a thread object
	std::queue<std::string> mQueue;									//!< Queue to store pending log entries
	static std::mutex		mMutex;									//!< Mutex for thread protection
	int						mMaxLogLevel = LOG_DEBUG;				//!< Allowed Maximum Logging Level
	int						mTimestampLevel = LOG_TS_USEC;			//!< Allowed Maximum Timestamp level
	bool					mConsoleOutputEnabled = true;			//!< Output to console enabled ? 
	bool					mFileOutputEnabled = true;				//!< Output to file enabled ?
	std::string				mOutputFile = "";						//!< Holds output file location.
	bool					mRunning = false;						//!< Track if Logger is running
	std::ofstream			mFile;									//!< File Stream To Write To
	std::string				mUser;									//!< System User for Log information location
};