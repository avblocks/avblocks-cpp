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

primo::ref<MediaSocket> createDecoderOutputSocket()
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

primo::ref<Transcoder> createWavWriter(const std::string &outputFile)
{
    // input stream infor, pin and socket
    auto inAsi = primo::make_ref(Library::createAudioStreamInfo());
    inAsi->setStreamType(StreamType::LPCM);
    inAsi->setChannels(2);
    inAsi->setSampleRate(48000);
    inAsi->setBitsPerSample(16);

    auto inPin = primo::make_ref(Library::createMediaPin());
    inPin->setStreamInfo(inAsi.get());

    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setStreamType(StreamType::LPCM);
    inSocket->pins()->add(inPin.get());

    // output stream info, pin and socket
    auto outfmt = primo::make_ref(Library::createAudioStreamInfo());
    outfmt->setStreamType(StreamType::LPCM);
    outfmt->setSampleRate(48000);
    outfmt->setChannels(2);
    outfmt->setBitsPerSample(16);

    auto outPin = primo::make_ref(Library::createMediaPin());
    outPin->setStreamInfo(outfmt.get());

    auto outSocket = primo::make_ref(Library::createMediaSocket());
    outSocket->setStreamType(StreamType::WAVE);
    outSocket->pins()->add(outPin.get());

    // set output file
    outSocket->setFile(primo::ustring(outputFile));

    // create transcoder
    auto wavWriter = primo::make_ref(Library::createTranscoder());
    wavWriter->setAllowDemoMode(true);
    wavWriter->inputs()->add(inSocket.get());
    wavWriter->outputs()->add(outSocket.get());

    return wavWriter;
}

bool decode(Options &opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.outputFile));

    // Create decoder transcoder
    auto decoder = primo::make_ref(Library::createTranscoder());
    decoder->setAllowDemoMode(true);

    auto inputSocket = primo::make_ref(Library::createMediaSocket());
    inputSocket->setFile(primo::ustring(opt.inputFile));
    decoder->inputs()->add(inputSocket.get());
    decoder->outputs()->add(createDecoderOutputSocket().get());

    if (!decoder->open())
    {
        printError("Decoder open", decoder->error());
        return false;
    }

    // Create WAV writer transcoder
    auto wavWriter = createWavWriter(opt.outputFile);
    if (!wavWriter->open())
    {
        printError("WAV Writer open", wavWriter->error());
        decoder->close();
        return false;
    }

    // Pull-push decoding loop
    int32_t decoderOutputIndex = 0;
    auto pcmSample = primo::make_ref(Library::createMediaSample());

    bool decoderEos = false;
    while (!decoderEos)
    {
        // Pull PCM sample from decoder
        if (decoder->pull(decoderOutputIndex, pcmSample.get()))
        {
            // Push PCM sample to WAV writer
            if (!wavWriter->push(0, pcmSample.get()))
            {
                printError("WAV Writer push", wavWriter->error());
                decoder->close();
                wavWriter->close();
                return false;
            }

            continue;
        }

        // No more PCM data from decoder
        const primo::error::ErrorInfo *error = decoder->error();
        if (error->facility() == primo::error::ErrorFacility::Codec &&
            error->code() == primo::codecs::CodecError::EOS)
        {
            // Push null to signal EOS to WAV writer
            wavWriter->push(0, nullptr);
            decoderEos = true;

            continue;
        }

        printError("Decoder pull", error);
        decoder->close();
        wavWriter->close();
        return false;
    }

    decoder->close();
    wavWriter->close();

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

    bool result = decode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}