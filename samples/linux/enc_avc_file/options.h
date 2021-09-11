/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#pragma once

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct ColorDescriptor
{
    primo::codecs::ColorFormat::Enum Id;
    const char * name;
    const char * description;
};

class FrameSize
{
public:
    FrameSize():width_(0), height_(0){}
    int width_;
    int height_;
};

struct Options
{
    Options(): help(false), list_colors(false) {}
    
    std::string yuv_file;
    std::string h264_file;
    FrameSize frame_size;
    ColorDescriptor yuv_color;
    double fps;

    bool help;
    bool list_colors;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);

