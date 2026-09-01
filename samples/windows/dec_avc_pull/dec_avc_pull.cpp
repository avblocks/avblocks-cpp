#include "stdafx.h"
#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

namespace av = primo::avblocks;
namespace pc = primo::codecs;

void printStatus(const primo::error::ErrorInfo* e)
{
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        wcout << L"Success";

    }
    else
    {
        if (e->message())
        {
            wcout << e->message() << L", ";
        }

        wcout << L"(facility:" << e->facility()
            << L" error:" << e->code() << L")" << endl;
    }

    wcout << endl;
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
    deleteFile(opt.outputFile.c_str());

    std::fstream outfile(opt.outputFile, std::ios::out | std::ios::binary);
    if (!outfile.is_open())
    {
        wcout << L"Could not open file " << opt.outputFile << endl;
        return false;
    }

    // create input socket from file
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setFile(opt.inputFile.c_str());

    // create output socket with one YUV 4:2:0 video pin
    auto outSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    if (!transcoder->open())
    {
        printError(L"Transcoder::open", transcoder->error());
        outfile.close();
        return false;
    }

    // decode by pulling decoded samples
    int32_t inputIndex = 0;
    auto yuvFrame = primo::make_ref(Library::createMediaSample());
    int32_t frameIndex = 0;

    while (transcoder->pull(inputIndex, yuvFrame.get()))
    {
        // Each call to Transcoder::pull returns a raw YUV 4:2:0 frame
        outfile.write(reinterpret_cast<const char*>(yuvFrame->buffer()->data()), yuvFrame->buffer()->dataSize());

        wcout << L"Frame Index: " << frameIndex << endl;
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
    outfile.close();

    wcout << L"Output: " << opt.outputFile << endl;
    wcout << L"Total frames: " << frameIndex << endl;

    return success;
}

int main(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
        case Parsed:  break;
    }

    stdout_utf16 utf16;

    Library::initialize();

    bool decodeResult = decodeAvcPull(opt);

    Library::shutdown();

    return decodeResult ? 0 : 1;
}
