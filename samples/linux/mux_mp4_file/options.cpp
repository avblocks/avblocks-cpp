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
    opt.input_audio.push_back(getExeDir() + "/../../assets/aud/big_buck_bunny_trailer.aud.mp4");
    opt.input_video.push_back(getExeDir() + "/../../assets/vid/big_buck_bunny_trailer.vid.mp4");
 
    fs::path output(getExeDir() + "/../../output/mux_mp4_file");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/big_buck_bunny_trailer.mp4";
    opt.output_file = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optionsConfig)
{
    cout << endl;
    cout << "Usage: mux_mp4_file --audio <input_AAC> --video <input_AVC> --output <output.mp4>" << endl;
    primo::program_options::doHelp(cout, optionsConfig);
}

bool validateOptions(Options& opt)
{    
    if (opt.input_audio.empty() && opt.input_video.empty())
    {
        return false;
    }

    if (opt.output_file.empty())
    {
        return false;
    }
    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        for(std::vector<string>::iterator input = opt.input_audio.begin(); input != opt.input_audio.end(); input++)
        {
            cout << " --audio " << *input;
        }
        for (std::vector<string>::iterator input = opt.input_video.begin(); input != opt.input_video.end(); input++)
        {
            cout << " --video " << *input;
        }
        cout << " --output " << opt.output_file;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
        ("help,?",			opt.help,								"")
        ("audio,a",         opt.input_audio,	vector<string>(),	"input AAC files. Can be used multiple times")
        ("video,v",         opt.input_video,    vector<string>(),   "input H264 files. Can be used multiple times")
        ("output,o",		opt.output_file,	string(),			"output file");

    try
    {
        primo::program_options::scanArgv(optionsConfig, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
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

