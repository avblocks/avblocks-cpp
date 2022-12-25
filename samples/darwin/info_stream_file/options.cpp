#include <iostream>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;

void setDefaultOptions(Options& opt)
{
    opt.avfile = getExeDir() + "/../../assets/mov/big_buck_bunny_trailer.mp4";
    cout << "Using default input file.\n";
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: info_stream_file --input <avfile>\n";
    primo::program_options::doHelp(cout, optcfg);
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --output " << opt.avfile;
        cout << endl;        
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input,i", opt.avfile, string(), "file; if no input is specified a default input file is used.");
    
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
    
    return Parsed;
}