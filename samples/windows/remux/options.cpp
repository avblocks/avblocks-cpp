#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;
using namespace primo::codecs;

void setDefaultOptions(Options& opt)
{
    //audio and video flags are enabled by default
    opt.inputFile = getExeDir() + L"/../../assets/mov/big_buck_bunny_trailer.mp4";
    
    fs::path output(getExeDir() + L"/../../output/remux");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/big_buck_bunny_trailer.mp4";

    opt.outputFile = s.str();
}

void help(OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << L"Usage: remux [--input inputFile.mp4] [--output outputFile.mp4] [--remux-audio yes|no] [--remux-video yes|no]\n" << endl;
    doHelp(wcout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    return (!opt.inputFile.empty() && 
            !opt.outputFile.empty());    
}


ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"--input " << opt.inputFile;
        wcout << L" --output " << opt.outputFile;
        wcout << L" --remux-video " << opt.reEncodeVideo;
        wcout << L" --remux-audio " << opt.reEncodeAudio;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",				opt.help,							L"")
        (L"input,i",			opt.inputFile,		wstring(),		L"input file")
        (L"output,o",			opt.outputFile,		wstring(),		L"output file")
        (L"remux-audio,a",	opt.reEncodeAudio,	YesNo(TRUE),	L"remux audio with yes|no")
        (L"remux-video,v",	opt.reEncodeVideo,	YesNo(TRUE),	L"remux video with yes|no");

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