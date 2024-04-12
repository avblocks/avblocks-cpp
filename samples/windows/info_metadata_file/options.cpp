#include "stdafx.h"

#include "util.h"

#include "program_options.h"
#include "options.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.avfile = getExeDir() + L"/../../assets/aud/Hydrate-Kenny_Beltrey.ogg";
}

void help(OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << endl << L"Usage: info_metadata_file --input <avfile>" << endl;
    doHelp(wcout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    return (!opt.avfile.empty());    
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        // default input file
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"--input " << opt.avfile;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",     opt.help,   L"")
        (L"input,i",    opt.avfile, wstring(), L"file; if no input is specified a default input file is used.");

    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (ParseFailure<wchar_t> &ex)
    {
        wcout << ex.message() << endl;
        help(optionsConfig);
        return Error;
    }

    if(opt.help)
    {
        help(optionsConfig);
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

    return Parsed;
}