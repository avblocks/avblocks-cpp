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

    socket->pins()->add(pin.get());

    return socket;
}

bool encode(Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.outputFile));

    std::ofstream outfile(opt.outputFile, ios_base::binary);
    if (!outfile.is_open())
    {
        cout << "Could not open file " << opt.outputFile << endl;
        return false;
    }

    // create input socket
    primo::ref<MediaSocket> inSocket (Library::createMediaSocket());
    inSocket->setFile(primo::ustring(opt.inputFile));
    
    // create output socket
    auto outSocket = createOutputSocket(opt);
    
    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());
    
    if (!transcoder->open())
    {
        printError("Transcoder::open", transcoder->error());
        return false;
    }

    // encode by pulling encoded samples
    int32_t outputIndex = 0;
    primo::ref<MediaSample> sample(Library::createMediaSample());
    while (transcoder->pull(outputIndex, sample.get()))
    {
        outfile.write((const char *)sample->buffer()->data(), sample->buffer()->dataSize());
    }

    const primo::error::ErrorInfo *error = transcoder->error();
    printError("Transcoder::pull", error);

    bool success = false;
    if ((error->facility() == primo::error::ErrorFacility::Codec) &&
        (error->code() == primo::codecs::CodecError::EOS))
    {
        // ok
        success = true;
    }

    transcoder->close();
    
    return success;
}

int main(int argc, char* argv[])
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