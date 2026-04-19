#include "util.h"
#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <locale.h>
#include <primo/platform/ustring.h>

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

void printError(const std::string& prefix, const primo::error::ErrorInfo* error)
{
    if (error)
    {
        std::cerr << prefix << " " << error->message() << " (code: " << error->code() << ")" << std::endl;
        
        std::string block = primo::ustring(error->block()).str();
        if (!block.empty())
            std::cerr << prefix << " Block: " << block << std::endl;
        
            std::string hint = primo::ustring(error->hint()).str();
        if (!hint.empty())
            std::cerr << prefix << " Hint: " << hint << std::endl;
    }
    else
    {
        std::cerr << prefix << " Unknown error" << std::endl;
    }
}
