/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <string>

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct PresetDescriptor
{
    //PresetDescriptor(): name(NULL), fileExtension(NULL) {}
    const char * name;
    const char * fileExtension;
};

struct Options
{
    Options() : help(false)
    {}

    PresetDescriptor preset;

    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);
