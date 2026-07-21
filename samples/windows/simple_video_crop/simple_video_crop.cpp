#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

using namespace primo;
using namespace primo::codecs;
using namespace primo::avblocks;

int wmain(int argc, wchar_t* argv[])
{
    Library::initialize();

    auto inputFile = primo::ustring(L"AndrewConnolly_2014.mp4");
    auto outputFile = primo::ustring(L"AndrewConnolly_2014_640x480.mp4");

    auto inputInfo = primo::make_ref(Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(inputFile);

    if (inputInfo->open()) {
        using namespace Param::Video;

        auto inputSocket = primo::make_ref(
            Library::createMediaSocket(inputInfo.get())
        );

        // Start with the same output as the input
        auto outputSocket = primo::make_ref(inputSocket->clone());
        outputSocket->setFile(outputFile);

        // Get the output video pin
        auto outVideoPin = outputSocket->pins()->at(0);

        auto outVideoStream = (VideoStreamInfo*)outVideoPin->streamInfo();

        // Set the new frame width and height to 640 x 480
        outVideoStream->setFrameWidth(640);
        outVideoStream->setFrameHeight(480);

        // Set the display ratio to 4:3
        outVideoStream->setDisplayRatioWidth(4);
        outVideoStream->setDisplayRatioHeight(3);

        // set Crop.Left and Crop.Right
        auto outVideoPinParams = primo::make_ref(Library::createParameterList());

        // The input video is 854x480.
        // To make it 640x480 we have to cut (854 - 640) / 2 pixels from each side.
        auto cropLeft = primo::make_ref(Library::createIntParameter());
        cropLeft->setName(Crop::Left);
        cropLeft->setValue((854 - 640) / 2);
        outVideoPinParams->add(cropLeft.get());

        auto cropRight = primo::make_ref(Library::createIntParameter());
        cropRight->setName(Crop::Right);
        cropRight->setValue((854 - 640) / 2);
        outVideoPinParams->add(cropRight.get());

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
            std::wcerr << L"transcoder->open() failed: "
                       << primo::ustring(transcoder->error()->message())
                       << std::endl;
        }
    } else {
        std::wcerr << L"inputInfo->open() failed: "
                    << primo::ustring(inputInfo->error()->message())
                    << std::endl;
    }

    Library::shutdown();
    return 0;
}
