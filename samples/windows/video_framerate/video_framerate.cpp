#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

bool changeVideoFrameRate(Options& opt)
{
    auto mediaInfo = primo::make_ref(Library::createMediaInfo());
    mediaInfo->inputs()->at(0)->setFile(opt.inputFile.c_str());

    if (!mediaInfo->open())
    {
        printError(L"MediaInfo open", mediaInfo->error());
        return false;
    }

    // create input socket
    auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));

    // create output socket
    auto outputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
    outputSocket->setFile(opt.outputFile.c_str());

    // get output video pin
    auto outVideoPin = outputSocket->pins()->at(0);

    // change the frame rate
    auto outVideoStream = static_cast<VideoStreamInfo*>(outVideoPin->streamInfo());
    outVideoStream->setFrameRate(opt.frameRate);

    // create a Transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);

    // add input and output sockets
    transcoder->inputs()->add(inputSocket.get());
    transcoder->outputs()->add(outputSocket.get());

    // process
    if (transcoder->open())
    {
        transcoder->run();
        transcoder->close();
    }
    else
    {
        printError(L"Transcoder open", transcoder->error());
        return false;
    }

    wcout << L"Output: " << opt.outputFile << endl;

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    Library::initialize();

    bool result = changeVideoFrameRate(opt);

    Library::shutdown();

    return result ? 0 : 1;
}
