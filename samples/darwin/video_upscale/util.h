#pragma once

#include "options.h"
#include <primo/avblocks/avb.h>
#include <string>

void printError(const std::string& prefix, const primo::error::ErrorInfo* error);
bool validateOptions(const Options& opt);
std::string getExeDir();
