#include <iostream>
#include <string>

#include <primo/avblocks/avb.h>

#include <primo/platform/reference++.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

bool upscaleVideo(Options& opt)
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
    
    // set the new frame width and height
    auto outVideoStream = static_cast<VideoStreamInfo*>(outVideoPin->streamInfo());
    outVideoStream->setFrameWidth(opt.width);
    outVideoStream->setFrameHeight(opt.height);
    
    // set the resize interpolation method:
    // InterpolationMethod::Cubic is best for upscaling (however, it is slow)
    auto outVideoPinParams = primo::make_ref(Library::createParameterList()); 

    auto interpolationMethod = primo::make_ref(Library::createIntParameter()); 
    interpolationMethod->setName(primo::avblocks::Param::Video::Resize::InterpolationMethod);
    interpolationMethod->setValue(primo::codecs::InterpolationMethod::Cubic);

    outVideoPinParams->add(interpolationMethod.get());

    outVideoPin->setParams(outVideoPinParams.get());
    
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
        
        if(!upscaleVideo(opt))
        {
            return 1;
        }
    }
    catch(const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    Library::shutdown();
    
    return 0;
}
