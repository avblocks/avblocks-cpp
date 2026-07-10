#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

#include <windows.h>

using namespace primo;
using namespace primo::codecs;
using namespace primo::avblocks;

int wmain(int argc, wchar_t* argv[])
{
    // needed for WMV
    CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    Library::initialize();

    auto inputFile = primo::ustring(L"Wildlife.wmv");
    auto outputFile = primo::ustring(L"Wildlife_h265_aac.mp4");

    auto inputInfo = primo::make_ref(Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(inputFile);

    if (inputInfo->open()) {
        auto inputSocket = primo::make_ref(
            Library::createMediaSocket(inputInfo.get())
        );
        
        // Start with MP4 / H.264 + AAC preset, then change video to H.265
        auto outputSocket = primo::make_ref(
            Library::createMediaSocket(Preset::Video::Generic::MP4::Base_H264_AAC)
        );

        // Get the output video stream info and modify it for H.265/HEVC
        auto outVideoStream = static_cast<VideoStreamInfo*>(outputSocket
                                                            ->pins()->at(0)
                                                            ->streamInfo());
        outVideoStream->setStreamType(StreamType::H265);
        outVideoStream->setStreamSubType(StreamSubType::HEVC_Annex_B);
        
        // With H.265/HEVC we can use lower bitrate, e.g. 500 kbps
        outVideoStream->setBitrate(500'000);

        // Change the output file name to Wildlife_h265_aac.mp4
        outputSocket->setFile(outputFile);
        
        // Create Transcoder and configure it with 
        // the input and output sockets
        auto transcoder = primo::make_ref(Library::createTranscoder());
        transcoder->inputs()->add(inputSocket.get());
        transcoder->outputs()->add(outputSocket.get());

        // Allow demo mode for the transcoder when 
        // using the demo version of the library
        transcoder->setAllowDemoMode(true);

        // Run the transcoder
        if (transcoder->open()) {
            transcoder->run();
            transcoder->close();
        } else {
            std::wcerr << L"transcoder->open() failed: " 
                       << primo::ustring(transcoder->error()->message()) 
                       << std::endl;
        }
    }

    Library::shutdown();

    CoUninitialize();

    return 0;
}
