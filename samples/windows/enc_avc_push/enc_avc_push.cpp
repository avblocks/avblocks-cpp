/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

class stdout_utf16
{
public:
    stdout_utf16()
    {
        _setmode(_fileno(stdout), _O_U16TEXT);
    }

    ~stdout_utf16()
    {
        _setmode(_fileno(stdout), _O_TEXT);
    }
};

void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        wcout << action << L": Success" << endl;
        return;
    }

    if (e->message())
    {
        wcout << action << L": " << e->message() << L" ";
    }

    wcout << L"(facility:" << e->facility() << L" error:" << e->code() << L")" << endl;
}

primo::ref<VideoStreamInfo> create_yuv_video_info(Options& opt)
{
    primo::ref<VideoStreamInfo> yuvVideo(Library::createVideoStreamInfo());

    yuvVideo->setStreamType(StreamType::UncompressedVideo);
    yuvVideo->setFrameRate(opt.fps);
    yuvVideo->setFrameWidth(opt.frame_size.width_);
    yuvVideo->setFrameHeight(opt.frame_size.height_);
    yuvVideo->setColorFormat(opt.yuv_color.Id);
    yuvVideo->setScanType(ScanType::Progressive);

    return yuvVideo;
}

primo::ref<Transcoder> create_yuv_reader(Options& opt)
{
    // Create VideoStreamInfo, MediaPin, and MediaSocket describing the YUV input.
    primo::ref<VideoStreamInfo> yuvInVideo = create_yuv_video_info(opt);

    primo::ref<MediaPin> yuvInPin(Library::createMediaPin());
    yuvInPin->setStreamInfo(yuvInVideo.get());

    primo::ref<MediaSocket> yuvInSocket(Library::createMediaSocket());
    yuvInSocket->setStreamType(StreamType::UncompressedVideo);
    yuvInSocket->setFile(opt.yuv_file.c_str());

    yuvInSocket->pins()->add(yuvInPin.get());

    // Create VideoStreamInfo, MediaPin, and MediaSocket describing the YUV output.
    // This is the same as the input, but no output file is set on the MediaSocket,
    // because we want to pull frames one by one using Transcoder::pull.
    primo::ref<VideoStreamInfo> yuvOutVideo = create_yuv_video_info(opt);

    primo::ref<MediaPin> yuvOutPin(Library::createMediaPin());
    yuvOutPin->setStreamInfo(yuvOutVideo.get());

    primo::ref<MediaSocket> yuvOutSocket(Library::createMediaSocket());
    yuvOutSocket->setStreamType(StreamType::UncompressedVideo);

    yuvOutSocket->pins()->add(yuvOutPin.get());

    // Create Transcoder.
    primo::ref<Transcoder> yuvReader(Library::createTranscoder());
    yuvReader->setAllowDemoMode(true);
    yuvReader->inputs()->add(yuvInSocket.get());
    yuvReader->outputs()->add(yuvOutSocket.get());

    return yuvReader;
}

primo::ref<Transcoder> create_h264_encoder(Options& opt)
{
    // Create VideoStreamInfo, MediaPin, and MediaSocket describing the YUV input.
    primo::ref<VideoStreamInfo> yuvInVideo = create_yuv_video_info(opt);

    primo::ref<MediaPin> yuvInPin(Library::createMediaPin());
    yuvInPin->setStreamInfo(yuvInVideo.get());

    primo::ref<MediaSocket> yuvInSocket(Library::createMediaSocket());
    yuvInSocket->setStreamType(StreamType::UncompressedVideo);

    yuvInSocket->pins()->add(yuvInPin.get());

    // Create VideoStreamInfo, MediaPin, and MediaSocket describing the H.264 output.
    primo::ref<VideoStreamInfo> h264OutVideo(Library::createVideoStreamInfo());
    h264OutVideo->setStreamType(StreamType::H264);
    h264OutVideo->setFrameRate(opt.fps);
    h264OutVideo->setFrameWidth(opt.frame_size.width_);
    h264OutVideo->setFrameHeight(opt.frame_size.height_);
    h264OutVideo->setColorFormat(opt.yuv_color.Id);
    h264OutVideo->setScanType(ScanType::Progressive);

    primo::ref<MediaPin> h264OutPin(Library::createMediaPin());
    h264OutPin->setStreamInfo(h264OutVideo.get());

    primo::ref<MediaSocket> h264OutSocket(Library::createMediaSocket());
    h264OutSocket->setStreamType(StreamType::H264);
    h264OutSocket->setFile(opt.h264_file.c_str());

    h264OutSocket->pins()->add(h264OutPin.get());

    // Transcoder.
    primo::ref<Transcoder> h264Encoder(Library::createTranscoder());
    h264Encoder->setAllowDemoMode(true);
    h264Encoder->inputs()->add(yuvInSocket.get());
    h264Encoder->outputs()->add(h264OutSocket.get());

    return h264Encoder;
}

bool encode_h264_stream(Transcoder* yuvReader, Transcoder* h264Encoder)
{
    int32_t inputIndex = 0;
    primo::ref<MediaSample> yuvFrame(Library::createMediaSample());

    while (true)
    {
        // Simulate a raw frame. Each call to Transcoder::pull returns one video frame.
        if (!yuvReader->pull(inputIndex, yuvFrame.get()))
            break;

        // Pass the raw video frame to Transcoder::push to encode it as AVC / H.264.
        if (!h264Encoder->push(0, yuvFrame.get()))
        {
            printError(L"H264 encoder push", h264Encoder->error());
            return false;
        }
    }

    const primo::error::ErrorInfo* error = yuvReader->error();
    if (error->facility() != primo::error::ErrorFacility::Codec ||
        error->code() != CodecError::EOS)
    {
        printError(L"YUV reader pull", error);
        return false;
    }

    if (!h264Encoder->flush())
    {
        printError(L"H264 encoder flush", h264Encoder->error());
        return false;
    }

    return true;
}

bool encode_h264_stream(Options& opt)
{
    // Transcoder will fail if the output file already exists.
    deleteFile(opt.h264_file.c_str());

    // Create a reader to simulate raw video frames. In reality, the raw video source
    // could be different, for example some kind of video capture device.
    primo::ref<Transcoder> yuvReader = create_yuv_reader(opt);

    // Create a H.264 encoder. We will pass the raw video frames to it to encode them as H.264.
    primo::ref<Transcoder> h264Encoder = create_h264_encoder(opt);

    if (!yuvReader->open())
    {
        printError(L"YUV reader open", yuvReader->error());
        return false;
    }

    bool success = false;
    if (h264Encoder->open())
    {
        success = encode_h264_stream(yuvReader.get(), h264Encoder.get());
        h264Encoder->close();
    }
    else
    {
        printError(L"H264 encoder open", h264Encoder->error());
    }

    yuvReader->close();

    return success;
}

int wmain(int argc, wchar_t* argv[])
{
    stdout_utf16 utf16;

    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
    case Command:
        return 0;
    case Error:
        return 1;
    case Parsed:
        break;
    }

    Library::initialize();
    bool result = encode_h264_stream(opt);
    Library::shutdown();

    return result ? 0 : 1;
}
