// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Log.h"
#include "CPP_Timer/Timer.h"

int main()
{
    std::string mUser = "Main";
    Essentials::Log* log = log->GetInstance();

    int init = log->Initialize("./OutputFiles/output");

    log->SetConsoleLogLevel(Essentials::LOG_LEVEL::LOG_ERROR);
    log->SetFileLogLevel(Essentials::LOG_LEVEL::LOG_INFO);
    log->SetLogTimestampLevel(Essentials::LOG_TIME::LOG_MSEC);
    log->LogToFile(true);
    log->AddEntry(Essentials::LOG_LEVEL::LOG_INFO, mUser, "Hello World, from %s %d", "Chip", 100);
    log->AddEntry(Essentials::LOG_LEVEL::LOG_DEBUG, mUser, "Debug Test");
    log->AddEntry(Essentials::LOG_LEVEL::LOG_ERROR, mUser, "Error Test");

    init = log->Initialize("./OutputFiles/output");

    log->ReleaseInstance();

    return 0;
}