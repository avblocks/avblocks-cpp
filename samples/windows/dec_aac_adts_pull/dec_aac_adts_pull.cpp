#include "stdafx.h"
#include "util.h"
#include "options.h"

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

primo::ref<Transcoder> createWavWriter(const wstring &outputFile)
{
    // input stream info, pin and socket
    auto infmt = primo::make_ref(Library::createAudioStreamInfo());
    infmt->setStreamType(StreamType::LPCM);
    infmt->setChannels(2);
    infmt->setSampleRate(48000);
    infmt->setBitsPerSample(16);

    auto inPin = primo::make_ref(Library::createMediaPin());
    inPin->setStreamInfo(infmt.get());

    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setStreamType(StreamType::LPCM);
    inSocket->pins()->add(inPin.get());

    // output stream info, pin and socket
    auto outfmt = primo::make_ref(infmt->clone());
    
    auto outPin = primo::make_ref(Library::createMediaPin());
    outPin->setStreamInfo(outfmt.get());

    auto outSocket = primo::make_ref(Library::createMediaSocket());
    outSocket->setStreamType(StreamType::WAVE);
    outSocket->pins()->add(outPin.get());

    // set output file
    outSocket->setFile(outputFile.c_str());

    // create transcoder
    auto wavWriter = primo::make_ref(Library::createTranscoder());
    wavWriter->setAllowDemoMode(TRUE);
    wavWriter->inputs()->add(inSocket.get());
    wavWriter->outputs()->add(outSocket.get());

    return wavWriter;
}

bool decode(Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    // Create decoder transcoder
    auto decoder = primo::make_ref(Library::createTranscoder());
    decoder->setAllowDemoMode(TRUE);

    auto inputSocket = primo::make_ref(Library::createMediaSocket());
    inputSocket->setFile(opt.inputFile.c_str());
    decoder->inputs()->add(inputSocket.get());
    decoder->outputs()->add(createDecoderOutputSocket().get());

    if (!decoder->open())
    {
        printError(L"Decoder open", decoder->error());
        return false;
    }

    // Create WAV writer transcoder
    auto wavWriter = createWavWriter(opt.outputFile);
    if (!wavWriter->open())
    {
        printError(L"WAV Writer open", wavWriter->error());
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
                printError(L"WAV Writer push", wavWriter->error());
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

        printError(L"Decoder pull", error);
        decoder->close();
        wavWriter->close();
        return false;
    }

    decoder->close();
    wavWriter->close();

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

    bool result = decode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}