#pragma once

enum ErrorCodes
{ 
    Parsed = 0,
    Error,
    Command,
};

struct PresetDescriptor
{
    const char* name;
    wchar_t * fileExtension;
};

struct Options
{
    Options() : help(false) {}

    PresetDescriptor preset;
    bool help;
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);

