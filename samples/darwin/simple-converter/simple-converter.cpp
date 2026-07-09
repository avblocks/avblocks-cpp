//
//  simple-converter.cpp
//  simple-converter
//

#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

using namespace primo;
using namespace primo::codecs;
using namespace primo::avblocks;

int main(int argc, const char *argv[]) {
    Library::initialize();

    auto inputInfo = primo::make_ref(Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(primo::ustring(L"Wildlife_h264_aac.mp4"));

    if (inputInfo->open()) {
        auto inputSocket = primo::make_ref(Library::createMediaSocket(inputInfo.get()));
        
        // Start with same output as the input, which is MP4 / H.264 + AAC
        auto outputSocket = primo::make_ref(inputSocket->clone());

        // Change the video stream type to H.265 (HEVC) and the stream subtype to HEVC_Annex_B
        auto outVideoStream = (VideoStreamInfo*)outputSocket->pins()->at(0)->streamInfo();
        outVideoStream->setStreamType(StreamType::H265);
        outVideoStream->setStreamSubType(StreamSubType::HEVC_Annex_B);
        
        // Set the bitrate to 500 kbps (500,000 bits per second)
        // Input is 700 kbps, so we are reducing the bitrate to 500 kbps for the output
        outVideoStream->setBitrate(500'000);

        // Change the output file name to Wildlife_h265_aac.mp4
        outputSocket->setFile(primo::ustring(L"Wildlife_h265_aac.mp4"));
        
        // Create Transcoder and configure it with the input and output sockets
        auto transcoder = primo::make_ref(Library::createTranscoder());
        transcoder->inputs()->add(inputSocket.get());
        transcoder->outputs()->add(outputSocket.get());

        // Allow demo mode for the transcoder when using the demo version of the library
        transcoder->setAllowDemoMode(true);

        // Run the transcoder
        if (transcoder->open()) {
            transcoder->run();
            transcoder->close();
        } else {
            std::cerr << "transcoder->open() failed: " 
                      << primo::ustring(transcoder->error()->message()) 
                      << std::endl;
        }
    }

    Library::shutdown();
    return 0;
}
