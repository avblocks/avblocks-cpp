/*
 *  Copyright (c)  Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include <unistd.h>

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include <primo/platform/reference++.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include <primo/avblocks/avb.h>

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

namespace fs = std::filesystem;
namespace av = primo::avblocks;
namespace pc = primo::codecs;

void printStatus(const primo::error::ErrorInfo* e)
{
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        cout << "Success";
        
    }
    else
    {
        if (e->message())
        {
            cout << primo::ustring(e->message()) << " ";
        }
        
        cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
    }
    
    cout << endl;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    vsi->setStreamType(StreamType::UncompressedVideo);
    vsi->setColorFormat(ColorFormat::YUV420);
    vsi->setScanType(ScanType::Progressive);

    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(vsi.get());
    
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::UncompressedVideo);
    socket->pins()->add(pin.get());
    
    return socket;
}

bool decodeAvcPull(Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.outputFile));

    ofstream outfile(opt.outputFile, ios_base::binary);
    if (!outfile.is_open())
    {
        cout << "Could not open file " << opt.outputFile << endl;
        return false;
    }

    // create input socket from file
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setFile(primo::ustring(opt.inputFile));

    // create output socket with one YUV 4:2:0 video pin
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

    // decode by pulling decoded samples
    int32_t inputIndex = 0;
    auto yuvFrame = primo::make_ref(Library::createMediaSample());
    int32_t frameIndex = 0;

    while (transcoder->pull(inputIndex, yuvFrame.get()))
    {
        // Each call to Transcoder::pull returns a raw YUV 4:2:0 frame
        outfile.write((const char *)yuvFrame->buffer()->data(), yuvFrame->buffer()->dataSize());
        
        cout << "Frame Index: " << frameIndex << endl;
        frameIndex++;
    }

    const primo::error::ErrorInfo* error = transcoder->error();
    printStatus(error);

    bool success = false;
    if ((error->facility() == primo::error::ErrorFacility::Codec) &&
        (error->code() == primo::codecs::CodecError::EOS))
    {
        // ok - end of stream
        success = true;
    }

    transcoder->close();

    cout << "Output: " << opt.outputFile << endl;
    cout << "Total frames: " << frameIndex << endl;

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
    
    bool decodeResult = decodeAvcPull(opt);
    
    Library::shutdown();
    
    return decodeResult ? 0 : 1;
}
