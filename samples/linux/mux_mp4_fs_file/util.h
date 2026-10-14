#pragma once

#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <strings.h>

#include <string>
#include <iostream>
#include <sstream>

#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

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

inline std::string getExeDir()
{
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len <= 0)
    {
        return std::string();
    }
    exe_path[len] = 0;

    return std::string(dirname(exe_path));
}
