#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void help(OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << L"Usage: enc_g711_alaw_file --input <wav file> --output <g711 alaw wav file>" << endl;
    doHelp(wcout, optionsConfig);
}

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        help(OptionsConfig<wchar_t>());
        return Error;
    }

    OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,h", opt.help, L"")
        (L"input,i", opt.inputFile, wstring(), L"input WAV file")
        (L"output,o", opt.outputFile, wstring(), L"output G.711 A-law WAV file");

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

    if (opt.help)
    {
        help(optionsConfig);
        return Command;
    }

    if (opt.inputFile.empty())
    {
        wcout << L"input file needed" << endl;
        help(optionsConfig);
        return Error;
    }

    if (opt.outputFile.empty())
    {
        wcout << L"output file needed" << endl;
        help(optionsConfig);
        return Error;
    }

    return Parsed;
}
