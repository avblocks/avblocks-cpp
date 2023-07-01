#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../../assets/aud/equinox-48KHz.wav";

    fs::path output(getExeDir() + "/../../output/enc_aac_adts_file");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/equinox-48KHz.adts.aac";
    opt.outputFile = s.str();
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "enc_aac_adts_file --input <wav file> --output <aac file>" << endl;
    doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    if(opt.inputFile.empty())
    {
        cout << "Input file needed" << endl;
        return false;
    }
    else
    {
        cout << "Input file: " << opt.inputFile << endl;
    }

    if(opt.outputFile.empty())
    {
        cout << "Output file needed" << endl;
        return false;
    }
    else
    {
        cout << "Output file: " << opt.outputFile << endl;
    }

    return true;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --output " << opt.outputFile;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    (("help,h"),    opt.help,                   (""))
    (("input,i"),   opt.inputFile,   string(),   ("input WAV file"))
    (("output,o"),  opt.outputFile,  string(),   ("output AAC file"));
       
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