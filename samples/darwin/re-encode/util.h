#pragma once

#include <primo/avblocks/AVBlocks.h>
#include <primo/platform/ErrorFacility.h>
#include <primo/platform/UString.h>

#include <string>
#include <iostream>
#include <sstream>

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

std::string getExeDir();
