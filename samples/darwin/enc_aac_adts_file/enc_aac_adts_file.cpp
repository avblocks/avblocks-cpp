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
    // create stream info to describe the output audio stream
    auto asi = primo::make_ref(Library::createAudioStreamInfo());
    asi->setStreamType(StreamType::AAC);
    asi->setStreamSubType(StreamSubType::AAC_ADTS);

    // You can change the sampling rate and the number of the channels
    // asi->setSampleRate(44100);
    // asi->setChannels(1);

    // create a pin using the stream info 
    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(asi.get());

    // finally create a socket for the output container format 
    // which in this cases is is AAC packaged as Audio Data Transport Stream (ATDS)
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::AAC);
    socket->setStreamSubType(StreamSubType::AAC_ADTS);

    socket->pins()->add(pin.get());

    // output to a file
    auto output_file = primo::ustring(opt.outputFile);
    socket->setFile(output_file);
    
    return socket;
}

bool encode(Options& opt)
{
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
    
    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.outputFile));
    
    if (!transcoder->open())
    {
        printError("Transcoder open", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError("Transcoder run", transcoder->error());
        return false;
    }

    transcoder->close();
    
    return true;
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