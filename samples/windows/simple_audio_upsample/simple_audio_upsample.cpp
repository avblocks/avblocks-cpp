#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

using namespace primo;
using namespace primo::codecs;
using namespace primo::avblocks;

int wmain(int argc, wchar_t* argv[])
{
    Library::initialize();

    auto inputFile = primo::ustring(L"kahvi011_kennybeltrey-hydrate.mp3");
    auto outputFile = primo::ustring(L"kahvi011_kennybeltrey-hydrate_48Khz.mp3");

    auto inputInfo = primo::make_ref(Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(inputFile);

    if (inputInfo->open()) {
        auto inputSocket = primo::make_ref(
            Library::createMediaSocket(inputInfo.get())
        );

        // Start with the same output as the input
        auto outputSocket = primo::make_ref(inputSocket->clone());
        outputSocket->setFile(outputFile);

        // Get the output audio pin
        auto outAudioPin = outputSocket->pins()->at(0);

        // Set output sampling rate to 48 KHz
        auto outAudioStream = (AudioStreamInfo*)outAudioPin->streamInfo();
        outAudioStream->setSampleRate(48000);

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
