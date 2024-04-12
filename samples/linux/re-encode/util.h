#pragma once

#include <string>
#include <sstream>

#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <strings.h>

// Mac OSX / Unix specific implementations

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

inline std::string getExeDir()
{	
   /*
     Linux:
    /proc/<pid>/exe

    Solaris:
    /proc/<pid>/object/a.out (filename only)
    /proc/<pid>/path/a.out (complete pathname)

    BSD:
    /proc/<pid>/file

    */

    pid_t pid = getpid();
    
    char proc_link[256];
    sprintf(proc_link,"/proc/%d/exe",pid);

    char exe_path[PATH_MAX];
    int len = readlink(proc_link, exe_path, sizeof(exe_path) - 1);
    if(len > 0)
    {
        exe_path[len] = 0;
    }
    else
    {
        return std::string();
    }
    
    char * exe_dir = dirname(exe_path);    
    return std::string(exe_dir);
}

inline void deleteFile(const char* file)
{
    remove(file);
}
