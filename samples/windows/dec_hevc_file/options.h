/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/
#pragma once

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct Options
{
    Options(): help(false) {}

    std::wstring inputFile;
    std::wstring outputFile;

    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);
