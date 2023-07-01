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


using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../../assets/vid/foreman_qcif.h264";

    fs::path output(getExeDir() + "/../../output/dec_avc_file");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/foreman_qcif.yuv";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: dec_avc_file -i <h264 file> -o <yuv file>\n";
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !(opt.inputFile.empty() || opt.outputFile.empty());    
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
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
            ("help,h", opt.help, "")
            ("input,i", opt.inputFile, string(), "H264 input file.")
            ("output,o", opt.outputFile, string(), "YUV output file.");
            
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