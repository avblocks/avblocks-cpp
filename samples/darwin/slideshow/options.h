#pragma once

#include <string>

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct PresetDescriptor
{
    const char * name;
    const char * fileExtension;
};

struct Options
{
    Options() : help(false)
    {}
    
    PresetDescriptor preset;
    
    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);

