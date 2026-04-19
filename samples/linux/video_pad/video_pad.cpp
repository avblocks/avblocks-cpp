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

bool padVideo(Options& opt)
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
    
    // set the output frame size
    auto outVideoStream = static_cast<VideoStreamInfo*>(outVideoPin->streamInfo());
    outVideoStream->setFrameWidth(opt.width);
    outVideoStream->setFrameHeight(opt.height);
    
    // set the padding parameters
    auto outVideoPinParams = primo::make_ref(Library::createParameterList());

    // set padding on all sides
    auto padLeft = primo::make_ref(Library::createIntParameter());
    padLeft->setName(Param::Video::Pad::Left);
    padLeft->setValue(opt.padLeft);
    outVideoPinParams->add(padLeft.get());

    auto padRight = primo::make_ref(Library::createIntParameter());
    padRight->setName(Param::Video::Pad::Right);
    padRight->setValue(opt.padRight);
    outVideoPinParams->add(padRight.get());

    auto padTop = primo::make_ref(Library::createIntParameter());
    padTop->setName(Param::Video::Pad::Top);
    padTop->setValue(opt.padTop);
    outVideoPinParams->add(padTop.get());

    auto padBottom = primo::make_ref(Library::createIntParameter());
    padBottom->setName(Param::Video::Pad::Bottom);
    padBottom->setValue(opt.padBottom);
    outVideoPinParams->add(padBottom.get());

    // set padding color (ARGB32 format)
    auto padColor = primo::make_ref(Library::createIntParameter());
    padColor->setName(Param::Video::Pad::Color);
    padColor->setValue(opt.padColor);
    outVideoPinParams->add(padColor.get());

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
        
        if(!padVideo(opt))
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
