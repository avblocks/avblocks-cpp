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
    inSocket->setFile(opt.inputFile.c_str());

    // create output socket
    auto outSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    
    // Continue to work even when license is invalid 
    // This is not recommended in production
    transcoder->setAllowDemoMode(TRUE);
    
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

    bool result = decode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}