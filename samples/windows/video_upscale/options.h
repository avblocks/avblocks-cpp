#pragma once

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct Options
{
    Options(): help(false), width(1920), height(1080) {}

    std::wstring inputFile;
    std::wstring outputFile;
    int width;
    int height;

    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);
