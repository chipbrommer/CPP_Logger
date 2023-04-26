// CPP_Logger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Log.h"

int main()
{
    std::string mUser = "Main";
    Log* log = log->GetInstance();

    int init = log->Initialize("./OutputFiles/output");

    log->SetConsoleLogLevel(LOG_LEVEL::LOG_ERROR);
    log->SetFileLogLevel(LOG_LEVEL::LOG_INFO);
    log->SetLogTimestampLevel(LOG_TIME::LOG_MSEC);
    log->LogToFile(true);
    log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "Hello World, from %s %d", "Chip", 100);
    log->AddEntry(LOG_LEVEL::LOG_DEBUG, mUser, "Debug Test");
    log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Error Test");
    log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "Log after sleeping for %d msecs", 1);
    log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "Repeat init test...");

    init = log->Initialize("./OutputFiles/output");

    log->ReleaseInstance();

    return 0;
}