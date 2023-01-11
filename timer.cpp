#include "timer.h"

static uint32_t tickOffset;
static bool initialized;

#ifdef _WIN32
static DWORD    WINAPI MsecThread(LPVOID);
static HANDLE timerThread;
static bool timerThreadReady;
static uint64_t usecStartTime;
static volatile uint32_t tickCount;
static double timerFactor;
#endif

static void Initialize(void);

/** Get current millisecond counter value

	The millisecond counter value increments once per millisecond, and
	starts from zero the first time this function is called. It
	wraps to zero when it reaches 2^32.

	On Linux, we need to ensure monotonicity by checking for negative
	time offsets; this is because on Linux we derive the millisecond count
	from the system time, and the ntpd service can step the system time
	backwards.
*/
uint32_t TIMER_GetMsecTicks(void)
{
	uint32_t now;
	bool firstTime;

	if (firstTime = !initialized)
	{
		Initialize();
	}

#ifdef _WIN32
	now = tickCount;
#else // Linux
	{
		static uint32_t prevNow;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		if (!firstTime)
		{
			int32_t elapsed = (int32_t)(now - prevNow);

			if (elapsed < 0)
			{
				tickOffset += elapsed;
				LOG_Message(LOG_WARN, "TIMER_GetMsecTicks: system time went backwards %d msec", -elapsed);
			}
		}
		prevNow = now;
	}
#endif

	if (firstTime)
	{
		tickOffset = now;
	}

	return now - tickOffset;
}

/** Get current microsecond counter value

	This is a high resolution counter whuch increments once per microsecond.
	The starting value is undefined. It wraps when it reaches 2^32.
*/
uint32_t TIMER_GetUsecTicks(void)
{

#ifdef _WIN32

	LARGE_INTEGER curCount;
	QueryPerformanceCounter(&curCount);
	return (uint32_t)((uint64_t)((((uint64_t)curCount.QuadPart - usecStartTime) * timerFactor + 0.5)) & 0xffffffff);

#else
	struct timeval tv;
	uint64_t usecs;

	gettimeofday(&tv, NULL);
	usecs = tv.tv_sec * 1000000 + tv.tv_usec;
	return (uint32_t)(usecs & 0xffffffff);
#endif
}

void TIMER_MsecSleep(uint32_t milliSecs)
{
#ifdef _WIN32
	Sleep(milliSecs);
#else
	usleep(milliSecs * 1000);
#endif
}

void TIMER_UsecSleep(uint32_t microSecs)
{
#ifdef _WIN32
	Sleep(microSecs / 1000);
#else
	usleep(microSecs);
#endif
}

void TIMER_Reset(void)
{
	uint32_t now;

	now = TIMER_GetMsecTicks();
	tickOffset += now;
}

#ifdef _WIN32

/*  Windows multimedia timer thread

	This thread facilitates true millisecond precision timing on Windows,
	by using the multimedia timer to increment a counter once every
	millisecond.
*/
static DWORD WINAPI MsecThread(LPVOID aParam)
{
	HANDLE eventHandle;
	MMRESULT timer;

	/* Set up the event which the timer will use to signal us. */
	eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (eventHandle == NULL)
	{
		return 0;
	}

	/* Request high-resolution timing. */
	timeBeginPeriod(1);

	/* Set up multimedia timer. */
	timer = timeSetEvent(1, 0, (LPTIMECALLBACK)eventHandle, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	if (timer == (MMRESULT)0)
	{
		timeEndPeriod(1);
		CloseHandle(eventHandle);
		return 0;
	}

	/* Let parent know we're running. */
	timerThreadReady = TRUE;

	/* Enter main timer processing loop. */
	for (;;)
	{
		/* Wait for callback to signal us. */
		DWORD result;
		result = WaitForSingleObject(eventHandle, 10);
		if (result == WAIT_OBJECT_0)
		{
			/* Increment tick count. */
			tickCount++;
		}
	}

	return 0;
}

void Fatal(std::string msg)
{
	fprintf(stderr, "%s\n", msg.c_str());
	exit(1);
}

#endif

/** Initialize timer subsystem
*/
static void Initialize(void)
{
#ifdef _WIN32
	LARGE_INTEGER hrInfo = { 0 };
	LARGE_INTEGER curCount = { 0 };
#endif
	if (initialized)
	{
		return;
	}

#ifdef _WIN32

	/* Set up high-res pollable timer. */
	if (!QueryPerformanceFrequency(&hrInfo)
		|| !QueryPerformanceCounter(&curCount))
	{
		Fatal("high resolution timer not available");
	}
	timerFactor = 1000000.0 / hrInfo.QuadPart;
	usecStartTime = (uint64_t)curCount.QuadPart;

	/* Launch millisecond timer thread. */
	timerThreadReady = FALSE;
	timerThread = CreateThread(NULL, 0, MsecThread, NULL, CREATE_SUSPENDED, 0);
	if (timerThread == NULL)
	{
		Fatal("failed to start timer thread");
	}

	if (!SetThreadPriority(timerThread, THREAD_PRIORITY_TIME_CRITICAL))
		Fatal("failed to set timer thread priority");
	if (ResumeThread(timerThread) == -1)
		Fatal("failed to resume timer thread");

	/* Wait for timer thread to get its act together. */
	while (!timerThreadReady)
		Sleep(1);

#endif

	initialized = TRUE;
}