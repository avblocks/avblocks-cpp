#pragma once

#include <primo/avblocks/AVBlocks.h>
#include <primo/platform/ErrorFacility.h>
#include <primo/platform/UString.h>

#include <string>
#include <iostream>
#include <sstream>

inline void printError(const char* action, const primo::error::ErrorInfo* e)
{
    using namespace std;
    
    if (action)
    {
        cout << action << ": ";
    }
    
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        cout << "Success" << endl;
        return;
    }
    
    if (e->message())
    {
        cout << primo::ustring(e->message()) << ", ";
    }
    
    cout << "facility:" << e->facility()
         << ", error:" << e->code()
         << ", hint:" << primo::ustring(e->hint())
         << endl;
}

inline void deleteFile(const char* file)
{
    remove(file);
}

std::string getExeDir();

