#include "stdafx.h"
#include "options.h"
#include "util.h"

using namespace primo::avblocks;
using namespace primo::codecs;
using namespace std;

bool MP4Mux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());

    primo::ref<Transcoder> transcoder(Library::createTranscoder());

    // Transcoder demo mode must be enabled,
    // in order to use the production release for testing (without a valid license)
    transcoder->setAllowDemoMode(true);

    // Use the first H.264 video stream and the first AAC audio stream found in the inputs.
    // All other streams are disabled.
    bool audioStreamDetected = false;
    bool videoStreamDetected = false;

    for (const auto& file : opt.input_files)
    {
        primo::ref<MediaInfo> mediaInfo(Library::createMediaInfo());
        mediaInfo->inputs()->at(0)->setFile(file.c_str());

        if (!mediaInfo->open())
        {
            printError(L"mediaInfo.open", mediaInfo->error());
            return false;
        }

        primo::ref<MediaSocket> inputSocket(Library::createMediaSocket(mediaInfo.get()));

        for (int j = 0; j < inputSocket->pins()->count(); j++)
        {
            MediaPin* pin = inputSocket->pins()->at(j);
            StreamType::Enum streamType = pin->streamInfo()->streamType();

            if (streamType == StreamType::H264 && !videoStreamDetected)
            {
                videoStreamDetected = true;
                wcout << L"Muxing video input: " << file << endl;
            }
            else if (streamType == StreamType::AAC && !audioStreamDetected)
            {
                audioStreamDetected = true;
                wcout << L"Muxing audio input: " << file << endl;
            }
            else
            {
                pin->setConnection(PinConnection::Disabled);
            }
        }

        transcoder->inputs()->add(inputSocket.get());
    }

    if (!videoStreamDetected && !audioStreamDetected)
    {
        wcout << L"No H264 video or AAC audio streams found in the input files." << endl;
        return false;
    }

    // Configure output
    primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
    outputSocket->setFile(opt.output_file.c_str());
    outputSocket->setStreamType(StreamType::MP4);

    if (videoStreamDetected)
    {
        primo::ref<VideoStreamInfo> streamInfo(Library::createVideoStreamInfo());
        streamInfo->setStreamType(StreamType::H264);
        streamInfo->setStreamSubType(StreamSubType::AVC1);

        primo::ref<MediaPin> pin(Library::createMediaPin());
        pin->setStreamInfo(streamInfo.get());
        outputSocket->pins()->add(pin.get());
    }

    if (audioStreamDetected)
    {
        primo::ref<AudioStreamInfo> streamInfo(Library::createAudioStreamInfo());
        streamInfo->setStreamType(StreamType::AAC);
        streamInfo->setStreamSubType(StreamSubType::AAC_MP4);

        primo::ref<MediaPin> pin(Library::createMediaPin());
        pin->setStreamInfo(streamInfo.get());
        outputSocket->pins()->add(pin.get());
    }

    // Fast start: run a second pass that moves the moov atom before the mdat atom,
    // so that the file can be played while it is being downloaded.
    if (opt.fast_start)
    {
        outputSocket->params()->addInt(Param::Muxer::MP4::FastStart, 1);
    }

    transcoder->outputs()->add(outputSocket.get());

    if (!transcoder->open())
    {
        printError(L"Open Transcoder", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError(L"Run Transcoder", transcoder->error());
        return false;
    }

    transcoder->close();

    wcout << L"Output file: " << opt.output_file << endl;

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    Library::initialize();

    // set your license string
    // Library::setLicense("PRIMO-LICENSE");

    bool mp4MuxResult = MP4Mux(opt);

    Library::shutdown();

    return mp4MuxResult ? 0 : 1;
}
