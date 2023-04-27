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
#include	<map>						// Mapping enum to strings
#include	"CPP_Timer/Timer.h"			// Timer class
//
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CPP_LOGGER					// Define the cpp logger class. 
#define     CPP_LOGGER
//

#if !defined CPP_TIMER && !defined OLD_TIMER
#define		NO_TIMER
#endif
//
// 
constexpr int MAX_LOG_MESSAGE_LENGTH = 250;	//! Maximum Loggable Message Length
//
///////////////////////////////////////////////////////////////////////////////

namespace Essentials
{
	// Levels of logging
	enum class LOG_LEVEL : const int
	{
		LOG_NONE,
		LOG_ERROR,
		LOG_WARN,
		LOG_INFO,
		LOG_DEBUG,
	};

	// Time stamp options
	enum class LOG_TIME : const int
	{
		LOG_NONE,
		LOG_MSEC,
		LOG_USEC,
	};

	// A Map to convert an logging level value to a readable string.
	static std::map<LOG_LEVEL, std::string> LevelMap
	{
		{LOG_LEVEL::LOG_NONE,	"NONE"},
		{LOG_LEVEL::LOG_ERROR,	"ERROR"},
		{LOG_LEVEL::LOG_WARN,	"WARN"},
		{LOG_LEVEL::LOG_INFO,	"INFO"},
		{LOG_LEVEL::LOG_DEBUG,	"DEBUG"},
	};

	// A Map to convert an error value to a readable string.
	static std::map<LOG_TIME, std::string> TimeMap
	{
		{LOG_TIME::LOG_NONE,	"NONE"},
		{LOG_TIME::LOG_MSEC,	"MSEC"},
		{LOG_TIME::LOG_USEC,	"USEC"},
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
		int		Initialize(std::string filename, bool enableConsoleLogging = true, bool enableFileLogging = true);

		//! @brief Adds a message into the queue to be logged
		//! @param level - Log level of the string.
		//! @param level - LOG Level of the string.
		//! @param user - User the message is coming from
		//! @param format - formatted string to be logged. 
		//! @return false if failed, true if message was logged
		bool	AddEntry(LOG_LEVEL level, std::string user, std::string format, ...);

		//! @brief Writes out the log entries. 
		void	WriteOut();

		//! @brief Sets the maximum logging level.
		//! @param level - Maximum level to be logged to console.
		//! @return false if failed, true if set
		bool    SetConsoleLogLevel(LOG_LEVEL level);

		//! @brief Sets the maximum logging level.
		//! @param level - Maximum level to be logged to file.
		//! @return false if failed, true if set
		bool    SetFileLogLevel(LOG_LEVEL level);

		//! @brief Sets the timestamp logging type
		//! @param tsLevel - time stamp type to be used in console log.
		//! @return false if failed, true if set
		bool    SetLogTimestampLevel(LOG_TIME tsLevel);

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
		//! @brief Hidden Constructor
		Log();

		//! @brief Hidden Deconstructor
		~Log();

		static Log* mInstance;												// Instance of Logger
		std::thread* mThread;												// Pointer to a thread object
		std::queue<std::string> mQueue;										// Queue to store pending log entries
		static std::mutex		mMutex;										// Mutex for thread protection
		LOG_LEVEL				mMaxConsoleLogLevel;						// Allowed Maximum Logging Level
		LOG_LEVEL				mMaxFileLogLevel;							// Allowed Maximum Logging Level
		LOG_TIME				mTimestampLevel;							// Allowed Maximum Timestamp level
		bool					mConsoleOutputEnabled;						// Output to console enabled ? 
		bool					mFileOutputEnabled;							// Output to file enabled ?
		std::string				mOutputFile;								// Holds output file location.
		bool					mRunning;									// Track if Logger is running
		std::ofstream			mFile;										// File Stream To Write To
		std::string				mUser;										// System User for Log information location
	};
}
#endif