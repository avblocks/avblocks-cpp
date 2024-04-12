#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.avfile = getExeDir() + L"/../../assets/mov/big_buck_bunny_trailer.mp4";
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: info_stream_file --input <avfile>\n";
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    return (!opt.avfile.empty());    
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.avfile;
        wcout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
            (L"help,h", opt.help, L"")
            (L"input,i", opt.avfile, wstring(), L"file, if no input is specified a default input file is used.");
       
    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t> &ex)
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