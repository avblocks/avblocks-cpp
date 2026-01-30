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

primo::ref<MediaSocket> createInputSocket()
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::LPCM);

    auto pin = primo::make_ref(Library::createMediaPin());
    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    asi->setStreamType(StreamType::LPCM);
    asi->setChannels(2);
    asi->setSampleRate(48000);
    asi->setBitsPerSample(16);
    
    pin->setStreamInfo(asi.get());
    socket->pins()->add(pin.get());

    return socket;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    // create stream info to describe the output audio stream
    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    asi->setStreamType(StreamType::MPEG_Audio);
    asi->setStreamSubType(StreamSubType::MPEG_Audio_Layer3);

    // The default bitrate is 128000. You can set it to 192000, 256000, etc.
    // asi->setBitrate(192000);

    // Optionally set the sampling rate and the number of the channels, e.g. 44.1 Khz, Mono 
    // asi->setSampleRate(44100);
    // asi->setChannels(1);

    // create a pin using the stream info 
    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(asi.get());

    // the pin allows you to specify additional parameters for the encoder 
    // for example, change the stereo mode, e.g. Joint Stereo
    // pin->params()->addInt(Param::Encoder::Audio::MPEG1::StereoMode, StereoMode::Joint);

    // finally create a socket for the output container format which is MP3 in this case
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::MPEG_Audio);
    socket->setStreamSubType(StreamSubType::MPEG_Audio_Layer3);
    socket->setFile(opt.outputFile.c_str());

    socket->pins()->add(pin.get());

    return socket;
}

primo::ref<Transcoder> createWavReader(const std::wstring& inputFile)
{
    auto wavReader = primo::make_ref(Library::createTranscoder());
    wavReader->setAllowDemoMode(true);
    
    auto wavInputSocket = primo::make_ref(Library::createMediaSocket());
    wavInputSocket->setFile(inputFile.c_str());
    wavReader->inputs()->add(wavInputSocket.get());
    
    auto pcmOutputSocket = primo::make_ref(Library::createMediaSocket());
    pcmOutputSocket->setStreamType(StreamType::LPCM);
    auto pcmPin = primo::make_ref(Library::createMediaPin());
    auto pcmAsi = primo::make_ref(Library::createAudioStreamInfo());
    pcmAsi->setStreamType(StreamType::LPCM);
    pcmAsi->setChannels(2);
    pcmAsi->setSampleRate(48000);
    pcmAsi->setBitsPerSample(16);
    pcmPin->setStreamInfo(pcmAsi.get());
    pcmOutputSocket->pins()->add(pcmPin.get());
    wavReader->outputs()->add(pcmOutputSocket.get());
    
    return wavReader;
}

bool encode(Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    // Create WAV reader transcoder
    auto wavReader = createWavReader(opt.inputFile);
    if (!wavReader->open())
    {
        printError(L"WAV Reader open", wavReader->error());
        return false;
    }

    // Create encoder transcoder
    auto encoder = primo::make_ref(Library::createTranscoder());
    encoder->setAllowDemoMode(true);
    encoder->inputs()->add(createInputSocket().get());
    encoder->outputs()->add(createOutputSocket(opt).get());
    
    if (!encoder->open())
    {
        printError(L"Encoder open", encoder->error());
        wavReader->close();
        return false;
    }

    // Push encoding loop
    int32_t wavOutputIndex = 0;
    auto pcmSample = primo::make_ref(Library::createMediaSample());
    
    bool wavEos = false;
    while (!wavEos)
    {
        // Get PCM sample from WAV reader
        if (wavReader->pull(wavOutputIndex, pcmSample.get()))
        {
            // Push PCM sample to encoder
            if (!encoder->push(0, pcmSample.get()))
            {
                printError(L"Encoder push", encoder->error());
                wavReader->close();
                encoder->close();
                return false;
            }
        }
        else
        {
            // No more PCM data from WAV reader
            const primo::error::ErrorInfo *error = wavReader->error();
            if (error->facility() == primo::error::ErrorFacility::Codec &&
                error->code() == primo::codecs::CodecError::EOS)
            {
                // Push null to signal EOS to encoder
                encoder->push(0, nullptr);
                wavEos = true;
            }
            else
            {
                printError(L"WAV Reader pull", error);
                wavReader->close();
                encoder->close();
                return false;
            }
        }
    }

    wavReader->close();
    encoder->close();
    
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
    }

    Library::initialize();

    bool result = encode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}
