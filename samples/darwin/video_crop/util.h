#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <primo/avblocks/avb.h>

void printError(const std::string& prefix, const primo::error::ErrorInfo* error);
std::string getExeDir();

inline void deleteFile(const char* file)
{
    remove(file);
}

#endif // UTIL_H
