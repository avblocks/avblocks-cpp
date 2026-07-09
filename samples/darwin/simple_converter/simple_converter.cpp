#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

using namespace primo;
using namespace primo::codecs;
using namespace primo::avblocks;

int main(int argc, const char *argv[]) {
    Library::initialize();

    auto inputFile = primo::ustring(L"Wildlife_h264_aac.mp4");
    auto outputFile = primo::ustring(L"Wildlife_h265_aac.mp4");

    auto inputInfo = primo::make_ref(Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(inputFile);

    if (inputInfo->open()) {
        auto inputSocket = primo::make_ref(
            Library::createMediaSocket(inputInfo.get())
        );
        
        // Start with same output as the input, which is MP4 / H.264 + AAC
        auto outputSocket = primo::make_ref(inputSocket->clone());

        // Get the output video stream info
        auto outVideoStream = (VideoStreamInfo*)outputSocket
                                                ->pins()->at(0)
                                                ->streamInfo();

        // Change the video stream type to H.265 (HEVC) 
        // and the stream subtype to HEVC Annex B
        outVideoStream->setStreamType(StreamType::H265);
        outVideoStream->setStreamSubType(StreamSubType::HEVC_Annex_B);
        
        // Input is H.264/AVC at 700 kbps
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
            std::cerr << "transcoder->open() failed: " 
                      << primo::ustring(transcoder->error()->message()) 
                      << std::endl;
        }
    }

    Library::shutdown();
    return 0;
}
