/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/
#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::program_options;

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: dump_hevc_au --input <h265-file> --output <folder>" << endl;
    doHelp(wcout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_file = getExeDir() + L"/../../assets/vid/foreman_qcif.h265";
    opt.output_dir = getExeDir() + L"/../../output/dump_hevc_au";
}

bool validateOptions(Options& opt)
{
    return !opt.input_file.empty() && !opt.output_dir.empty();
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_file;
        wcout << L" --output " << opt.output_dir;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,       L"")
        (L"input,i",  opt.input_file, wstring(), L"input file (HEVC/H.265)")
        (L"output,o", opt.output_dir, wstring(), L"output directory");

    try
    {
        scanArgv(optcfg, argc, argv);
    }
    catch (ParseFailure<wchar_t>& ex)
    {
        wcout << ex.message() << endl;
        help(optcfg);
        return Error;
    }

    if (opt.help)
    {
        help(optcfg);
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }

    return Parsed;
}
