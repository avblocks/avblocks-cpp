#pragma once

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct Options
{
    Options(): help(false), frameRate(30.0) {}

    std::wstring inputFile;
    std::wstring outputFile;
    double frameRate;

    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);
