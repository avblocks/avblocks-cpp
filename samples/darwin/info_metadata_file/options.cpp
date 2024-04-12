/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include <string>
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../assets/aud/Hydrate-Kenny_Beltrey.ogg";
    cout << "Using default option: info_metadata_file --input " << opt.inputFile << endl << endl;
}

void printUsage(OptionsConfig<char>& optionsConfig)
{
    cout << endl << "Usage: info_metadata_file --input <file>" << endl;
    doHelp(cout, optionsConfig);
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if(argc == 1)
    {
        setDefaultOptions(opt);
        return Parsed;
    }
    OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
    (("help,h"), opt.help, (""))
    (("input,i"), opt.inputFile, string(), ("file; if no input is specified a default input file is used."));
    
    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        printUsage(optionsConfig);
        return Error;
    }
    
    if(opt.help)
    {
        printUsage(optionsConfig);
        return Command;
    }
    
    return Parsed;
}
