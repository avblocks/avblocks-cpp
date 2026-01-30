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

#include <memory.h>

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(primo::ustring(opt.outputFile));
    socket->setStreamType(StreamType::WAVE);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    pin->setStreamInfo(asi.get());

    asi->setStreamType(StreamType::LPCM);

    // You can change the number of the channels
    //asi->setChannels(1);
    
    // or the sampling rate and
    //asi->setSampleRate(44100);

    return socket;
}

bool decode(Options& opt)
{
    primo::ref<MediaSocket> inSocket(Library::createMediaSocket());
    inSocket->setFile(primo::ustring(opt.inputFile));

    // create output socket
    auto outSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    
    // Continue to work even when license is invalid 
    // This is not recommended in production
    transcoder->setAllowDemoMode(true);
    
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    if (!transcoder->open())
    {
        printError("Transcoder open", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError("Transcoder run", transcoder->error());

        transcoder->close();
        return false;
    }

    transcoder->close();
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

    bool result = decode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}
