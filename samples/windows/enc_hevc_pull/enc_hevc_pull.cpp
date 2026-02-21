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

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

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

primo::ref<MediaSocket> createInputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::UncompressedVideo);
    socket->setFile(opt.yuv_file.c_str());

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    vsi->setStreamType(StreamType::UncompressedVideo);
    vsi->setFrameWidth(opt.frame_size.width_);
    vsi->setFrameHeight(opt.frame_size.height_);
    vsi->setColorFormat(opt.yuv_color.Id);
    vsi->setFrameRate(opt.fps);
    vsi->setScanType(ScanType::Progressive);

    return socket;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::H265);
    socket->setStreamSubType(StreamSubType::HEVC_Annex_B);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    vsi->setStreamType(StreamType::H265);
    vsi->setStreamSubType(StreamSubType::HEVC_Annex_B);

    return socket;
}

bool encode(Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.h265_file.c_str());

    ofstream outfile(opt.h265_file, ios_base::binary);
    if (!outfile.is_open())
    {
        wcout << L"Could not open file " << opt.h265_file << endl;
        return false;
    }

    // create input socket
    auto inSocket = createInputSocket(opt);

    // create output socket
    auto outSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    if (!transcoder->open())
    {
        printError(L"Transcoder::open", transcoder->error());
        return false;
    }

    // encode by pulling encoded samples
    int32_t outputIndex = 0;
    auto sample = primo::make_ref(Library::createMediaSample());
    while (transcoder->pull(outputIndex, sample.get()))
    {
        outfile.write((const char*)sample->buffer()->data(), sample->buffer()->dataSize());
    }

    const primo::error::ErrorInfo* error = transcoder->error();
    printError(L"Transcoder::pull", error);

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

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
    }

    Library::initialize();

    bool encodeResult = encode(opt);

    Library::shutdown();

    return encodeResult ? 0 : 1;
}
