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

bool WebMMux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());

    auto transcoder = primo::make_ref(Library::createTranscoder());

    // Transcoder demo mode must be enabled, 
    // in order to use the production release for testing (without a valid license)
    transcoder->setAllowDemoMode(true);

    auto outputSocket = primo::make_ref(Library::createMediaSocket());
    outputSocket->setFile(opt.output_file.c_str());
    outputSocket->setStreamType(StreamType::WebM);

    // audio
    for(int i = 0; i < (int)opt.input_audio.size(); i++)
    {
        auto outputPin = primo::make_ref(Library::createMediaPin());
        auto asi = primo::make_ref(Library::createAudioStreamInfo());
        asi->setStreamType(StreamType::Vorbis);
        outputPin->setStreamInfo(asi.get());

        outputSocket->pins()->add(outputPin.get());

        auto inputSocket = primo::make_ref(Library::createMediaSocket());
        inputSocket->setFile(opt.input_audio[i].c_str());
        inputSocket->setStreamType(StreamType::WebM);
        transcoder->inputs()->add(inputSocket.get());

        wcout << L"Muxing audio input: " << opt.input_audio[i] << endl;
    }

    // video
    for (int i = 0; i < (int)opt.input_video.size(); i++)
    {
        auto outputPin = primo::make_ref(Library::createMediaPin());
        auto vsi = primo::make_ref(Library::createVideoStreamInfo());
        vsi->setStreamType(StreamType::VP8);
        outputPin->setStreamInfo(vsi.get());

        outputSocket->pins()->add(outputPin.get());

        auto inputSocket = primo::make_ref(Library::createMediaSocket());
        inputSocket->setFile(opt.input_video[i].c_str());
        inputSocket->setStreamType(StreamType::WebM);
        transcoder->inputs()->add(inputSocket.get());

        wcout << L"Muxing video input: " << opt.input_video[i] << endl;
    }

    transcoder->outputs()->add(outputSocket.get());

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
        
    wcout << L"Output file: " << opt.output_file << endl;
        
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    Library::initialize();

    // set your license string
    // Library::setLicense(L"PRIMO-LICENSE");

    bool webmMuxResult = WebMMux(opt);

    Library::shutdown();

    return webmMuxResult ? 0 : 1;
}
