/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include <primo/avblocks/avb.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

/*
command line options:

long format:
    dump_hevc_au --input <h265-file> --output <folder>

short format:
    dump_hevc_au -i <h265-file> -o <folder>

*/

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: dump_hevc_au --input <h265-file> --output <folder>" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_file = getExeDir() + "/../../assets/vid/foreman_qcif.h265";
    opt.output_dir = getExeDir() + "/../../output/dump_hevc_au";
}

bool validateOptions(Options& opt)
{
    return !opt.input_file.empty() && !opt.output_dir.empty();
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_file;
        cout << " --output " << opt.output_dir;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
        ("help,h", opt.help, "")
        ("input,i", opt.input_file, string(), "input file (HEVC/H.265)")
        ("output,o", opt.output_dir, string(), "output directory");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        help(optcfg);
        return Error;
    }

    if(opt.help)
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
