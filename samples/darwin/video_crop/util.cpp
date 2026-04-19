#include "util.h"
#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <locale.h>
#include <primo/platform/ustring.h>

void printError(const std::string& prefix, const primo::error::ErrorInfo* error)
{
    if (error)
    {
        std::cerr << prefix << ": " << primo::ustring(error->block()).str() << std::endl;
        if (error->hint())
        {
            std::cerr << "Hint: " << primo::ustring(error->hint()).str() << std::endl;
        }
    }
}

std::string getExeDir()
{
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path)-1);
    if (len != -1)
    {
        path[len] = '\0';
        return std::string(path).substr(0, std::string(path).find_last_of('/'));
    }
    return ".";
}
