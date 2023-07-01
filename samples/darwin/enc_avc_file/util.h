#pragma once

#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include <string>
#include <iostream>
#include <sstream>

inline void deleteFile(const char* file)
{
    remove(file);
}

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

std::string getExeDir();
