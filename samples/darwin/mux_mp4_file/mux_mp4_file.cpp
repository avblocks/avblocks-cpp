#include <primo/avblocks/AVBlocks.h>

#include <primo/platform/Reference++.h>
#include <primo/platform/ErrorFacility.h>
#include <primo/platform/UString.h>

#include "util.h"
#include "options.h"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <memory.h>

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

bool MP4Mux(const Options& opt)
{
    deleteFile(primo::ustring(opt.output_file));

    primo::ref<Transcoder> transcoder (Library::createTranscoder());

    // Transcoder demo mode must be enabled, 
    // in order to use the production release for testing (without a valid license)
    transcoder->setAllowDemoMode(true);

    primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
    outputSocket->setFile(primo::ustring(opt.output_file));
    outputSocket->setStreamType(StreamType::MP4);

    // audio
    for(int i = 0; i < (int)opt.input_audio.size(); i++)
    {
        primo::ref<MediaPin> outputPin(Library::createMediaPin());
        primo::ref<AudioStreamInfo> asi(Library::createAudioStreamInfo());
        asi->setStreamType(StreamType::AAC);
        outputPin->setStreamInfo(asi.get());

        outputSocket->pins()->add(outputPin.get());

        primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
        inputSocket->setFile(primo::ustring(opt.input_audio[i]));
        inputSocket->setStreamType(StreamType::MP4);
        transcoder->inputs()->add(inputSocket.get());

        cout << "Muxing audio input: " << opt.input_audio[i] << endl;
    }

    // video
    for (int i = 0; i < (int)opt.input_video.size(); i++)
    {
        primo::ref<MediaPin> outputPin(Library::createMediaPin());
        primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
        vsi->setStreamType(StreamType::H264);
        outputPin->setStreamInfo(vsi.get());

        outputSocket->pins()->add(outputPin.get());

        primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
        inputSocket->setFile(primo::ustring(opt.input_video[i]));
        inputSocket->setStreamType(StreamType::MP4);
        transcoder->inputs()->add(inputSocket.get());

        cout << "Muxing video input: " << opt.input_video[i] << endl;
    }

    transcoder->outputs()->add(outputSocket.get());

    if (!transcoder->open())
    {
        printError("Open Transcoder", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError("Run Transcoder", transcoder->error());
        return false;
    }

    transcoder->close();
        
    cout << "Output file: " << opt.output_file << endl;
        
    return true;
}

int main(int argc, char* argv[])
{
    Options opt;

    switch(prepareOptions( opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    Library::initialize();

    // set your license string
    // Library::setLicense("PRIMO-LICENSE");

    bool mp4MuxResult = MP4Mux(opt);

    Library::shutdown();

    return mp4MuxResult ? 0 : 1;
}

