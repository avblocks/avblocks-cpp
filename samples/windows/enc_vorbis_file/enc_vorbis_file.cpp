/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

class stdout_utf16
{
public:
    stdout_utf16()
    {
        // change stdout to Unicode. Cyrillic and Ideographic characters will appear in the console (console font is unicode).
        _setmode(_fileno(stdout), _O_U16TEXT);
    }

    ~stdout_utf16()
    {
        // restore ANSI mode
        _setmode(_fileno(stdout), _O_TEXT);
    }
};

void printStatus(const primo::error::ErrorInfo *e)
{
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        wcout << L"Success";
    }
    else
    {
        wcout << L"facility: " << e->facility() << L", error: " << e->code();

        if (e->message())
        {
            wcout << L", " << e->message();
        }

        if (e->hint())
        {
            wcout << L", " << e->hint();
        }
    }

    wcout << endl;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    // create stream info to describe the output audio stream
    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    asi->setStreamType(StreamType::Vorbis);

    // The default bitrate is 128000. You can set it to 192000, 256000, etc.
    // asi->setBitrate(192000);

    // Optionally set the sampling rate and the number of the channels, e.g. 44.1 Khz, Mono 
    // asi->setSampleRate(44100);
    // asi->setChannels(1);

    // create a pin using the stream info 
    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(asi.get());

    // finally create a socket for the output container format which is OGG in this case
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::OGG);

    socket->pins()->add(pin.get());

    // output to a file
    socket->setFile(opt.outputFile.c_str());

    return socket;
}

bool encode(Options& opt)
{
    // create input socket
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setFile(opt.inputFile.c_str());
    
    // create output socket
    auto outSocket = createOutputSocket(opt);
    
    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());
    
    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());
    
    wcout << L"Transcoder open: ";
    if (!transcoder->open())
    {
        printStatus(transcoder->error());
        return false;
    }
    printStatus(transcoder->error());

    wcout << L"Transcoder run: ";
    if (!transcoder->run())
    {
        printStatus(transcoder->error());
        return false;
    }
    printStatus(transcoder->error());

    transcoder->close();
    wcout << L"Transcoder close: ";
    printStatus(transcoder->error());
    
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;
    
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:	return 1;
        case Parsed: break;
    }
    
    Library::initialize();
    
    bool result = encode(opt);
    
    Library::shutdown();
    
    return result ? 0 : 1;
}
