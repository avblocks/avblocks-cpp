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
    auto outputFile = primo::ustring(L"AndrewConnolly_2014_640x480_Padded.mp4");

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

        // set Pad.Top and Pad.Bottom
        auto outVideoPinParams = primo::make_ref(Library::createParameterList());

        // The input video is 16:9, 854 x 480,
        // to squeeze it in 640 x 480 and keep the 16:9 display ratio,
        // the height has to be: 640 * 9 / 16

        // Pad by (480 - 640 * 9 / 16) / 2 pixels on each side.
        auto padTop = primo::make_ref(Library::createIntParameter());
        padTop->setName(Pad::Top);
        padTop->setValue((480 - 640 * 9 / 16) / 2);
        outVideoPinParams->add(padTop.get());

        auto padBottom = primo::make_ref(Library::createIntParameter());
        padBottom->setName(Pad::Bottom);
        padBottom->setValue((480 - 640 * 9 / 16) / 2);
        outVideoPinParams->add(padBottom.get());

        // The padding will trigger downscaling,
        // so we also set the interpolation method,
        // for best result when downscaling, use InterpolationMethod::Super
        auto interpolationMethod = primo::make_ref(Library::createIntParameter());
        interpolationMethod->setName(Resize::InterpolationMethod);
        interpolationMethod->setValue(InterpolationMethod::Super);
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
