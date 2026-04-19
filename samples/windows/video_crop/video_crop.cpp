#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

bool cropVideo(Options& opt)
{
    auto mediaInfo = primo::make_ref(Library::createMediaInfo());
    mediaInfo->inputs()->at(0)->setFile(opt.inputFile.c_str());

    if (!mediaInfo->open())
    {
        printError(L"MediaInfo open", mediaInfo->error());
        return false;
    }

    // create input socket
    auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));

    // create output socket
    auto outputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
    outputSocket->setFile(opt.outputFile.c_str());

    // get output video pin
    auto outVideoPin = outputSocket->pins()->at(0);

    // update the output frame dimensions to reflect the crop
    auto outVideoStream = static_cast<VideoStreamInfo*>(outVideoPin->streamInfo());
    int newWidth  = outVideoStream->frameWidth()  - opt.cropLeft - opt.cropRight;
    int newHeight = outVideoStream->frameHeight() - opt.cropTop  - opt.cropBottom;
    outVideoStream->setFrameWidth(newWidth);
    outVideoStream->setFrameHeight(newHeight);

    // update the display ratio to match the new frame dimensions
    int g = std::gcd(newWidth, newHeight);
    outVideoStream->setDisplayRatioWidth(newWidth  / g);
    outVideoStream->setDisplayRatioHeight(newHeight / g);

    // set the crop parameters
    auto outVideoPinParams = primo::make_ref(Library::createParameterList());

    auto cropLeft = primo::make_ref(Library::createIntParameter());
    cropLeft->setName(Param::Video::Crop::Left);
    cropLeft->setValue(opt.cropLeft);
    outVideoPinParams->add(cropLeft.get());

    auto cropRight = primo::make_ref(Library::createIntParameter());
    cropRight->setName(Param::Video::Crop::Right);
    cropRight->setValue(opt.cropRight);
    outVideoPinParams->add(cropRight.get());

    auto cropTop = primo::make_ref(Library::createIntParameter());
    cropTop->setName(Param::Video::Crop::Top);
    cropTop->setValue(opt.cropTop);
    outVideoPinParams->add(cropTop.get());

    auto cropBottom = primo::make_ref(Library::createIntParameter());
    cropBottom->setName(Param::Video::Crop::Bottom);
    cropBottom->setValue(opt.cropBottom);
    outVideoPinParams->add(cropBottom.get());

    outVideoPin->setParams(outVideoPinParams.get());

    // delete output file if it already exists
    deleteFile(opt.outputFile.c_str());

    // create output directory if needed
    std::filesystem::create_directories(std::filesystem::path(opt.outputFile).parent_path());

    // create a Transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);

    // add input and output sockets
    transcoder->inputs()->add(inputSocket.get());
    transcoder->outputs()->add(outputSocket.get());

    // process
    if (transcoder->open())
    {
        transcoder->run();
        transcoder->close();
    }
    else
    {
        printError(L"Transcoder open", transcoder->error());
        return false;
    }

    wcout << L"Output: " << opt.outputFile << endl;

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    Library::initialize();

    bool result = cropVideo(opt);

    Library::shutdown();

    return result ? 0 : 1;
}
