#include <iostream>
#include <string>
#include <filesystem>
#include <numeric>

#include <primo/avblocks/avb.h>

#include <primo/platform/reference++.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

bool cropVideo(Options& opt)
{
    auto mediaInfo = primo::make_ref(Library::createMediaInfo());
    mediaInfo->inputs()->at(0)->setFile(primo::ustring(opt.inputFile));
    
    if(!mediaInfo->open())
    {
        printError("MediaInfo open:", mediaInfo->error());
        return false;
    }
    
    // create input socket
    auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
    
    // create output socket
    auto outputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
    outputSocket->setFile(primo::ustring(opt.outputFile));
    
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

    // set crop on all sides
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
    if(transcoder->open())
    {
        transcoder->run();
        transcoder->close();
    }
    else
    {
        printError("Transcoder open:", transcoder->error());
        return false;
    }
    
    cout << "Output: " << opt.outputFile << endl;
    
    return true;
}

int main(int argc, char* argv[])
{
    Library::initialize();
    
    try
    {
        Options opt;
        ErrorCodes ec = prepareOptions(opt, argc, argv);
        
        if(ec != Parsed)
        {
            return 1;
        }
        
        if(!cropVideo(opt))
        {
            return 1;
        }
    }
    catch(const exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
