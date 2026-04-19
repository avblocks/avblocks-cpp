#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <primo/avblocks/avb.h>

void printError(const std::string& prefix, const primo::error::ErrorInfo* error);
std::string getExeDir();

#endif // UTIL_H
