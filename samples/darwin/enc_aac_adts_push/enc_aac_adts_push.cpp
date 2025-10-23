#include <primo/avblocks/avb.h>

#include <primo/platform/reference++.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include "util.h"
#include "options.h"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

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

primo::ref<MediaSocket> createOutputSocket(const std::string& outputFile)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::AAC);
    socket->setStreamSubType(StreamSubType::AAC_ADTS);
    socket->setFile(primo::ustring(outputFile));

    auto pin = primo::make_ref(Library::createMediaPin());
    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    asi->setStreamType(StreamType::AAC);
    asi->setStreamSubType(StreamSubType::AAC_ADTS);

    // You can change the sampling rate and the number of the channels
    // asi->setChannels(1);
    // asi->setSampleRate(44100);

    pin->setStreamInfo(asi.get());
    socket->pins()->add(pin.get());

    return socket;
}

primo::ref<Transcoder> createWavReader(const std::string& inputFile)
{
    auto wavReader = primo::make_ref(Library::createTranscoder());
    wavReader->setAllowDemoMode(true);
    
    auto wavInputSocket = primo::make_ref(Library::createMediaSocket());
    wavInputSocket->setFile(primo::ustring(inputFile));
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

bool encode(Options &opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.outputFile));

    // Create WAV reader transcoder
    auto wavReader = createWavReader(opt.inputFile);
    if (!wavReader->open())
    {
        printError("WAV Reader open", wavReader->error());
        return false;
    }

    // Create encoder transcoder
    auto encoder = primo::make_ref(Library::createTranscoder());
    encoder->setAllowDemoMode(true);
    encoder->inputs()->add(createInputSocket().get());
    encoder->outputs()->add(createOutputSocket(opt.outputFile).get());
    
    if (!encoder->open())
    {
        printError("Encoder open", encoder->error());
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
                printError("Encoder push", encoder->error());
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
                printError("WAV Reader pull", error);
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

int main(int argc, char *argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
    case Command:
        return 0;
    case Error:
        return 1;
    case Parsed:
        break;
    }

    Library::initialize();

    bool result = encode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}