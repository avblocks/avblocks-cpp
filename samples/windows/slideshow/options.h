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
    wchar_t * extension;
};

struct Options
{
    Options() : help(false) {}

    // input options
    std::wstring input_dir;

    // output options
    PresetDescriptor preset;
    std::wstring output_file;


    bool help;
    bool list_presets;
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);

