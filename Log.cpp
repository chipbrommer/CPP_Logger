///////////////////////////////////////////////////////////////////////////////
//!
//! @file		Log.cpp
//! 
//! @brief		Implementation of singleton Log class 
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
#include	"Log.h"						// Log Class
//
///////////////////////////////////////////////////////////////////////////////

// Initialize static class variables.
Log* Log::mInstance = NULL;
std::mutex Log::mMutex;

Log* Log::GetInstance()
{
	std::lock_guard<std::mutex> lock(Log::mMutex);
	if (mInstance == NULL)
	{
		mInstance = new Log;
	}

	return mInstance;
}

void Log::ReleaseInstance()
{
	if (mInstance != NULL)
	{
		delete mInstance;
		mInstance = NULL;
	}
}

int Log::Initialize(std::string filename, bool enableConsoleLogging, bool enableFileLogging)
{
	// Catch if already initialized. 
	if (mRunning)
	{
		return 0;
	}

	uint32_t initStart = TIMER_GetMsecTicks();

	this->mUser = "Log";
	this->mOutputFile = filename;
	this->mConsoleOutputEnabled = enableConsoleLogging;
	this->mFileOutputEnabled = enableFileLogging;

	size_t i = filename.rfind('/', filename.length());
	if (i == std::string::npos)
	{
		printf_s("log path is not a valid path.\n");
		return -1;
	}
	std::string directoryPath = filename.substr(0, i);

	// make the directory if it doesn't exist
	struct stat st = { 0 };
	if (stat(directoryPath.c_str(), &st) == -1)
	{
		int made = 0;
#ifdef _WIN32
		made = _mkdir(directoryPath.c_str());
#else
		made = mkdir(directoryPath.c_str(), 0777);
#endif
		if (made == -1)
		{
			char buffer[256];
			strerror_s(buffer, sizeof(buffer), errno); // get string message from errno, XSI-compliant version
			printf_s("Error %s\n", buffer);
		}
	}

	// Create the datetime stamp for the file creation
	auto now = std::chrono::system_clock::now();
	char time_str[] = "yyy.mm.dd.HH-MM.SS.fff";
	time_t ttime_t = std::chrono::system_clock::to_time_t(now);
	std::tm ttm = { 0 };
	localtime_s(&ttm, &ttime_t);
	strftime(time_str, strlen(time_str), "%Y.%m.%d-%H.%M.%S", &ttm);
	std::chrono::system_clock::time_point tp_sec = std::chrono::system_clock::from_time_t(ttime_t);
	int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - tp_sec).count();

	std::string milliseconds = std::to_string(ms);
	if (milliseconds.length() < 3)
	{
		milliseconds.insert(0, 3 - milliseconds.length(), '0');
	}

	// Create the file and verify its open - if successful start the writing thread.
	mFile.open(filename + "_" + std::string(time_str) + "." + milliseconds + ".txt");
	if (!mFile.is_open())
	{
		printf_s("Error creating log file [%s].\n", filename.c_str());
	}
	else
	{
		mThread = new std::thread(&Log::WriteOut, this);
	}

	// Successful initialization
	mRunning = true;
	AddEntry(LOG_INFO, mUser, "Initialize Complete: Start time: %d \t End Time: %d", initStart, TIMER_GetMsecTicks());
	return 1;
}

bool Log::AddEntry(int level, std::string user, std::string format, ...)
{
	va_list args;
	char msg[MAX_LOG_MESSAGE_LENGTH + 1];
	char ts[20];

	// Return if level of this message exceeds the maximum level.
	if (level > mMaxLogLevel)
	{
		return false;
	}

	// Format the message timestamp
	switch (mTimestampLevel)
	{
		case LOG_TS_MSEC:
		{
			// TODO - fix new timer class
			//snprintf(ts, sizeof(ts), "[%7u] ", (unsigned int)dTimer.GetMSecTicks());
			snprintf(ts, sizeof(ts), "[%7u] ", (unsigned int)TIMER_GetMsecTicks());
			break;
		}
		case LOG_TS_USEC:
		{
			uint32_t t;
			// TODO - fix new timer class
			//t = dTimer.GetUSecTicks();
			t = TIMER_GetUsecTicks();
			snprintf(ts, sizeof(ts), "[%7u.%03u] ", t / 1000, t % 1000);
			break;
		}
		default:
		{
	#ifdef _WIN32
			strcpy_s(ts, sizeof(ts), "");
	#else
			strcpy(ts, "");
	#endif
		}
	}

	// Format the message with args
	va_start(args, format);
#ifdef _WIN32
	vsnprintf_s(msg, sizeof(msg), MAX_LOG_MESSAGE_LENGTH, format.c_str(), args);
#else
	vsnprintf(msg, LOG_MAX_MSG_LEN, format.c_str(), args);
#endif
	msg[sizeof(msg) - 1] = '\0';
	va_end(args);

	// Log to console if enabled
	if (mConsoleOutputEnabled)
	{
#ifdef _WIN32
		char buf[20000];
		snprintf(buf, sizeof(buf), "%s - %s - %s\n", ts, user.c_str(), msg);
		OutputDebugStringA(buf);    // goes to the debug console
		printf_s("%s", buf);
#else
		printf("%s - %s - %s\n", ts, user, msg);
#endif
	}

	// Log to file if enabled
	if (mFileOutputEnabled)
	{
		mMutex.lock();
		char buffer[20000];
		snprintf(buffer, sizeof(buffer), "%s - %s - %s", ts, user.c_str(), msg);
		std::string tmp = buffer;
		mQueue.push(tmp);
		mMutex.unlock();
	}

	return true;
}

void Log::WriteOut()
{
	while (mRunning)
	{
		std::string entry;

		mMutex.lock();
		if (mFile.is_open() && !mQueue.empty())
		{
			entry = mQueue.front();
			mQueue.pop();
		}
		mMutex.unlock();

		if (entry.length() > 0)
		{
			mFile << entry << "\n";
			mFile.flush();

			entry.clear();
		}

		// TODO - fix new timer class
		//dTimer.MSecSleep(1);
		TIMER_MsecSleep(1);
	}
}

bool Log::SetLogLevel(int level)
{
	mMaxLogLevel = level;
	return (level == mMaxLogLevel);
}

bool Log::SetLogTimestampLevel(int tsLevel)
{
	mTimestampLevel = tsLevel;
	return (tsLevel == mTimestampLevel);
}

bool Log::LogToConsole(bool enable)
{
	mConsoleOutputEnabled = enable;
	return (enable == mConsoleOutputEnabled);
}

bool Log::LogToFile(bool enable)
{
	mFileOutputEnabled = enable;
	return (enable == mFileOutputEnabled);
}

Log::~Log()
{
	// Notify close and wait for thread to finish writing to file
	AddEntry(LOG_INFO, mUser, "Closing.");
	while (!mQueue.empty()) {}

	mRunning = false;
	mThread->join();
	delete mThread;
	mFile.close();
}

Log::Log(){}