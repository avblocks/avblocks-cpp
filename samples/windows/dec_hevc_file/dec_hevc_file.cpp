/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/
#include "stdafx.h"
#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
    if (action)
    {
        wcout << action << L": ";
    }

    if (primo::error::ErrorFacility::Success == e->facility())
    {
        wcout << L"Success" << endl;
        return;
    }

    if (e->message())
    {
        wcout << e->message() << L", ";
    }

    wcout << L"facility:" << e->facility() << L", error:" << e->code() << endl;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    vsi->setStreamType(StreamType::UncompressedVideo);
    vsi->setColorFormat(ColorFormat::YUV420);

    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(vsi.get());

    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(opt.outputFile.c_str());
    socket->setStreamType(StreamType::UncompressedVideo);
    socket->pins()->add(pin.get());

    return socket;
}

bool decodeHevcFile(Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    auto mediaInfo = primo::make_ref(Library::createMediaInfo());
    mediaInfo->inputs()->at(0)->setFile(opt.inputFile.c_str());

    if (!mediaInfo->open())
    {
        printError(L"MediaInfo open:", mediaInfo->error());
        return false;
    }

    // create input socket
    auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));

    // create output socket
    auto outputSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inputSocket.get());
    transcoder->outputs()->add(outputSocket.get());

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

    wcout << L"Output: " << opt.outputFile << endl;

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
    }

    Library::initialize();

    bool decodeResult = decodeHevcFile(opt);

    Library::shutdown();

    return decodeResult ? 0 : 1;
}
