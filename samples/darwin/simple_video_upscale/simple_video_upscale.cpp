#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

using namespace primo;
using namespace primo::codecs;
using namespace primo::avblocks;

int main(int argc, const char *argv[]) {
    Library::initialize();

    auto inputFile = primo::ustring(L"AndrewConnolly_2014.mp4");
    auto outputFile = primo::ustring(L"AndrewConnolly_2014_1080p.mp4");

    auto inputInfo = primo::make_ref(Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(inputFile);

    if (inputInfo->open()) {
        auto inputSocket = primo::make_ref(
            Library::createMediaSocket(inputInfo.get())
        );

        // Start with the same output as the input
        auto outputSocket = primo::make_ref(inputSocket->clone());
        outputSocket->setFile(outputFile);

        // Get the output video pin
        auto outVideoPin = outputSocket->pins()->at(0);

        // Set the new frame width and height to Full HD 1920x1080
        auto outVideoStream = (VideoStreamInfo*)outVideoPin->streamInfo();
        outVideoStream->setFrameWidth(1920);
        outVideoStream->setFrameHeight(1080);

        // Set the resize interpolation method:
        // InterpolationMethod::Cubic is best for upscaling (however, it is slow)
        auto outVideoPinParams = primo::make_ref(Library::createParameterList());

        auto interpolationMethod = primo::make_ref(Library::createIntParameter());
        interpolationMethod->setName(Param::Video::Resize::InterpolationMethod);
        interpolationMethod->setValue(InterpolationMethod::Cubic);
        outVideoPinParams->add(interpolationMethod.get());

        outVideoPin->setParams(outVideoPinParams.get());

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
    } else {
        std::cerr << "inputInfo->open() failed: "
                   << primo::ustring(inputInfo->error()->message())
                   << std::endl;
    }

    Library::shutdown();
    return 0;
}
