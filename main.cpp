// CPP_Logger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Log.h"

int main()
{
    std::string mUser = "Main";
    Log* log = log->GetInstance();
    log->Initialize("./OutputFiles/output");
    log->SetLogLevel(LOG_INFO);
    log->SetLogTimestampLevel(LOG_TS_USEC);
    log->LogToFile(true);

    log->AddEntry(LOG_INFO, mUser, "Hello World, from %s %d", "Chip", 100);
    log->AddEntry(LOG_DEBUG, mUser, "Debug Test");
    TIMER_MsecSleep(1);

    log->AddEntry(LOG_INFO, mUser, "Log after sleeping for %d msecs", 1);

    log->ReleaseInstance();
}