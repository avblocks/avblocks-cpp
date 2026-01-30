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

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(opt.outputFile.c_str());
    socket->setStreamType(StreamType::MPEG_Audio);
    socket->setStreamSubType(StreamSubType::MPEG_Audio_Layer3);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    pin->setStreamInfo(asi.get());

    asi->setStreamType(StreamType::MPEG_Audio);
    asi->setStreamSubType(StreamSubType::MPEG_Audio_Layer3);

    // Change the encoding bitrate with the following line. The default bitrate is 128000. You can set it to 192000, 256000, etc.
    // asi->setBitrate(192000);

    // Change the sampling rate and the number of the channels, e.g. Mono, 44.1 Khz
    // asi->setChannels(1);
    // asi->setSampleRate(44100);

    // Change the stereo mode, e.g. Joint Stereo
    // pin->params()->addInt(Param::Encoder::Audio::MPEG1::StereoMode, StereoMode::Joint);

    return socket;
}

bool encode(Options& opt)
{
    primo::ref<MediaSocket> inSocket(Library::createMediaSocket());
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

    if (!transcoder->open())
    {
        printError(L"Transcoder open", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError(L"Transcoder run", transcoder->error());
        return false;
    }

    transcoder->close();

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions( opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
    }

    Library::initialize();

    bool result = encode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}