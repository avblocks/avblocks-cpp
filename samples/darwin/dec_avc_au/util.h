#pragma once

#include <primo/avblocks/AVBlocks.h>
#include <primo/platform/ErrorFacility.h>
#include <primo/platform/UString.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

inline void deleteFile(const char* file)
{
    remove(file);
}

inline std::vector<uint8_t> readFileBytes(const char *name)
{
    std::ifstream f(name, std::ios::binary);
    std::vector<uint8_t> bytes;
    if (f)
    {
        f.seekg(0, std::ios::end);
        size_t filesize = f.tellg();
        bytes.resize(filesize);
        f.seekg(0, std::ios::beg);
        f.read((char*)&bytes[0], filesize);
    }
    
    return bytes;
}

std::string getExeDir();