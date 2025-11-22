# avblocks-cpp

AVBlocks C++ SDK (CLI Samples)

# Usage

## Quick Start

### Library Initialization

AVBlocks must be initialized before use and shut down when done:

```cpp
#include <primo/avblocks/avb.h>

int main() {
    // Initialize AVBlocks
    primo::avblocks::Library::initialize();
    
    // Set license (optional for demo)
    // primo::avblocks::Library::setLicense("YOUR-LICENSE-KEY");
    
    // Enable demo mode for testing without license
    transcoder->setAllowDemoMode(true);
    
    // Your code here
    
    // Shutdown AVBlocks
    primo::avblocks::Library::shutdown();
    
    return 0;
}
```

## Core Concepts

### MediaSocket

A `MediaSocket` represents an input or output container/file with specific media parameters. It acts as a collection of media streams (via `MediaPin` objects) and defines the container format.

```cpp
// Create a media socket
primo::ref<MediaSocket> socket(Library::createMediaSocket());

// Set file path
socket->setFile(primo::ustring("video.mp4"));

// Set container type
socket->setStreamType(StreamType::MP4);

// Create socket from preset
auto socket = primo::make_ref(Library::createMediaSocket("mp4.h264.aac"));
```

### MediaPin

A `MediaPin` represents a single media stream (audio or video) within a `MediaSocket`. Each pin has a `StreamInfo` object that describes the stream's properties (codec, resolution, sample rate, etc.).

```cpp
// Create a pin for video
primo::ref<MediaPin> pin(Library::createMediaPin());

// Create and configure video stream info
primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::H264);
vsi->setFrameWidth(1920);
vsi->setFrameHeight(1080);
vsi->setFrameRate(30.0);

// Attach stream info to pin
pin->setStreamInfo(vsi.get());

// Add pin to socket
socket->pins()->add(pin.get());

// Access pins from a socket
MediaPinList* pins = socket->pins();
int pinCount = pins->count();
MediaPin* firstPin = pins->at(0);
```

### StreamInfo

`StreamInfo` and its subclasses (`VideoStreamInfo`, `AudioStreamInfo`) describe the properties of a media stream:

```cpp
// Video stream info
primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::H264);
vsi->setStreamSubType(StreamSubType::AVC_Annex_B);
vsi->setFrameWidth(1920);
vsi->setFrameHeight(1080);
vsi->setFrameRate(30.0);
vsi->setColorFormat(ColorFormat::YUV420);
vsi->setScanType(ScanType::Progressive);
vsi->setBitrate(2000000); // 2 Mbps

// Audio stream info
primo::ref<AudioStreamInfo> asi(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::AAC);
asi->setSampleRate(44100);
asi->setChannels(2);
asi->setBitsPerSample(16);
asi->setBitrate(128000); // 128 kbps

// Get info properties
MediaType::Enum mediaType = si->mediaType(); // Video or Audio
StreamType::Enum streamType = si->streamType();
double duration = si->duration();
int id = si->ID();
```

### MediaInfo

`MediaInfo` is used to probe media files and extract information about containers, streams, and metadata without performing transcoding.

```cpp
// Create MediaInfo object
primo::ref<MediaInfo> info(Library::createMediaInfo());

// Set input file
info->inputs()->at(0)->setFile(primo::ustring("video.mp4"));

// Open and analyze the file
if (info->open()) {
    // Access container information
    MediaSocket* socket = info->outputs()->at(0);
    StreamType::Enum containerType = socket->streamType();
    
    // Iterate through streams
    for (int i = 0; i < socket->pins()->count(); i++) {
        MediaPin* pin = socket->pins()->at(i);
        StreamInfo* si = pin->streamInfo();
        
        if (si->mediaType() == MediaType::Video) {
            VideoStreamInfo* vsi = static_cast<VideoStreamInfo*>(si);
            int width = vsi->frameWidth();
            int height = vsi->frameHeight();
            double fps = vsi->frameRate();
            double duration = vsi->duration();
            ColorFormat::Enum colorFormat = vsi->colorFormat();
            ScanType::Enum scanType = vsi->scanType();
        }
        else if (si->mediaType() == MediaType::Audio) {
            AudioStreamInfo* asi = static_cast<AudioStreamInfo*>(si);
            int sampleRate = asi->sampleRate();
            int channels = asi->channels();
            int bitsPerSample = asi->bitsPerSample();
            int bitrate = asi->bitrate();
        }
    }
}

// Create input socket from MediaInfo
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket(info.get()));
```

### Transcoder

The `Transcoder` class performs media conversion between input and output sockets. It supports three modes of operation:

1. **Run Mode**: Simple one-shot transcoding - `transcoder->run()`
2. **Push Mode**: Feed data to the transcoder manually - `transcoder->push()`
3. **Pull Mode**: Extract encoded/decoded data frame by frame - `transcoder->pull()`

```cpp
primo::ref<Transcoder> transcoder(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    printError("Transcoder open", transcoder->error());
    return false;
}
```

### MediaSample and MediaBuffer

`MediaSample` represents a single unit of media data (audio frame or video frame) with timing information. `MediaBuffer` holds the actual media data.

```cpp
// Create a media sample
primo::ref<MediaSample> sample(Library::createMediaSample());

// Create and attach a buffer
primo::ref<MediaBuffer> buffer(Library::createMediaBuffer(dataSize));
uint8_t* data = buffer->start();
memcpy(data, sourceData, dataSize);
buffer->setData(0, dataSize);
sample->setBuffer(buffer.get());

// Set timestamps (in seconds)
sample->setStartTime(0.0);

// Attach external buffer without copy
std::vector<uint8_t> externalData(1024);
buffer->attach(externalData.data(), externalData.size(), true);

// Read buffer data
const uint8_t* readData = sample->buffer()->data();
int readSize = sample->buffer()->dataSize();
```

## Common Patterns

### Getting Media Information

Extract stream information from a media file:

```cpp
#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>

using namespace primo::avblocks;
using namespace primo::codecs;

primo::ref<MediaInfo> info(Library::createMediaInfo());
info->inputs()->at(0)->setFile(primo::ustring("input.mp4"));

if (info->open()) {
    MediaSocket* socket = info->outputs()->at(0);
    
    // Container type
    StreamType::Enum containerType = socket->streamType();
    std::cout << "Container: " << getStreamTypeName(containerType) << std::endl;
    
    // Iterate through all streams
    for (int i = 0; i < socket->pins()->count(); i++) {
        MediaPin* pin = socket->pins()->at(i);
        StreamInfo* si = pin->streamInfo();
        
        std::cout << "Stream #" << i << " " << getMediaTypeName(si->mediaType()) << std::endl;
        
        if (si->mediaType() == MediaType::Video) {
            VideoStreamInfo* vsi = static_cast<VideoStreamInfo*>(si);
            std::cout << "  Frame size: " << vsi->frameWidth() << "x" << vsi->frameHeight() << std::endl;
            std::cout << "  Frame rate: " << vsi->frameRate() << std::endl;
            std::cout << "  Duration: " << vsi->duration() << " seconds" << std::endl;
            std::cout << "  Bitrate: " << vsi->bitrate() << std::endl;
            std::cout << "  Color format: " << getColorFormatName(vsi->colorFormat()) << std::endl;
            std::cout << "  Scan type: " << getScanTypeName(vsi->scanType()) << std::endl;
        }
        else if (si->mediaType() == MediaType::Audio) {
            AudioStreamInfo* asi = static_cast<AudioStreamInfo*>(si);
            std::cout << "  Sample rate: " << asi->sampleRate() << std::endl;
            std::cout << "  Channels: " << asi->channels() << std::endl;
            std::cout << "  Bits per sample: " << asi->bitsPerSample() << std::endl;
            std::cout << "  Bitrate: " << asi->bitrate() << std::endl;
            std::cout << "  Duration: " << asi->duration() << " seconds" << std::endl;
        }
    }
}
```

### Decoding Video (Run Mode)

Decode H.264/AVC video to raw YUV format using simple run mode:

```cpp
using namespace primo::avblocks;
using namespace primo::codecs;

// Use MediaInfo to get input properties
primo::ref<MediaInfo> info(Library::createMediaInfo());
info->inputs()->at(0)->setFile(primo::ustring("input.h264"));

if (!info->open()) {
    printError("MediaInfo open", info->error());
    return false;
}

// Create input socket from MediaInfo
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket(info.get()));

// Create output socket for YUV
auto outputSocket = primo::make_ref(Library::createMediaSocket());
outputSocket->setFile(primo::ustring("output.yuv"));
outputSocket->setStreamType(StreamType::UncompressedVideo);

auto pin = primo::make_ref(Library::createMediaPin());
auto vsi = primo::make_ref(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::UncompressedVideo);
vsi->setColorFormat(ColorFormat::YUV420);
pin->setStreamInfo(vsi.get());
outputSocket->pins()->add(pin.get());

// Create and configure transcoder
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    printError("Transcoder open", transcoder->error());
    return false;
}

// Simple run - transcodes entire file
if (!transcoder->run()) {
    printError("Transcoder run", transcoder->error());
    return false;
}

transcoder->close();
```

### Decoding Video (Pull Mode)

Decode video and extract frames one by one:

```cpp
// Setup input and output sockets (same as run mode)
// ...

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Pull decoded frames one by one
int32_t outputIndex = 0;
auto sample = primo::make_ref(Library::createMediaSample());

while (transcoder->pull(outputIndex, sample.get())) {
    // Process decoded frame
    MediaBuffer* buffer = sample->buffer();
    const uint8_t* data = buffer->data();
    int dataSize = buffer->dataSize();
    
    // Write to file or process further
    outputFile.write((const char*)data, dataSize);
}

// Check if we reached end of stream
const primo::error::ErrorInfo* error = transcoder->error();
if (error->facility() == primo::error::ErrorFacility::Codec &&
    error->code() == primo::codecs::CodecError::EOS) {
    // Successfully reached end of stream
    std::cout << "Success" << std::endl;
}
else {
    printError("Transcoder pull", error);
}

transcoder->close();
```

### Encoding Video (Run Mode)

Encode raw YUV video to H.264/AVC:

```cpp
// Create input socket for YUV
auto inputSocket = primo::make_ref(Library::createMediaSocket());
inputSocket->setStreamType(StreamType::UncompressedVideo);
inputSocket->setFile(primo::ustring("input.yuv"));

auto inPin = primo::make_ref(Library::createMediaPin());
auto inVsi = primo::make_ref(Library::createVideoStreamInfo());
inVsi->setStreamType(StreamType::UncompressedVideo);
inVsi->setColorFormat(ColorFormat::YUV420);
inVsi->setFrameWidth(1920);
inVsi->setFrameHeight(1080);
inVsi->setFrameRate(30.0);
inVsi->setScanType(ScanType::Progressive);
inPin->setStreamInfo(inVsi.get());
inputSocket->pins()->add(inPin.get());

// Create output socket for H.264
auto outputSocket = primo::make_ref(Library::createMediaSocket());
outputSocket->setFile(primo::ustring("output.h264"));
outputSocket->setStreamType(StreamType::H264);

auto outPin = primo::make_ref(Library::createMediaPin());
auto outVsi = primo::make_ref(Library::createVideoStreamInfo());
outVsi->setStreamType(StreamType::H264);
outVsi->setStreamSubType(StreamSubType::AVC_Annex_B);
outPin->setStreamInfo(outVsi.get());
outputSocket->pins()->add(outPin.get());

// Create and run transcoder
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

if (!transcoder->run()) {
    return false;
}

transcoder->close();
```

### Encoding Video (Pull Mode)

Encode video and extract encoded packets frame by frame:

```cpp
// Setup input and output sockets
// ...

std::ofstream outfile("output.h264", std::ios::binary);

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Pull encoded samples
int32_t outputIndex = 0;
auto sample = primo::make_ref(Library::createMediaSample());

while (transcoder->pull(outputIndex, sample.get())) {
    // Write encoded data
    outfile.write((const char*)sample->buffer()->data(), 
                  sample->buffer()->dataSize());
}

const primo::error::ErrorInfo* error = transcoder->error();
if (error->facility() == primo::error::ErrorFacility::Codec &&
    error->code() == primo::codecs::CodecError::EOS) {
    // Success
}

transcoder->close();
outfile.close();
```

### Encoding Video (Push Mode)

Feed raw frames to encoder manually:

```cpp
// Output socket for H.264
// ...

// Input socket without file (we'll push data manually)
auto inputSocket = primo::make_ref(Library::createMediaSocket());
inputSocket->setStreamType(StreamType::UncompressedVideo);

auto pin = primo::make_ref(Library::createMediaPin());
auto vsi = primo::make_ref(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::UncompressedVideo);
vsi->setColorFormat(ColorFormat::YUV420);
vsi->setFrameWidth(1920);
vsi->setFrameHeight(1080);
vsi->setFrameRate(30.0);
vsi->setScanType(ScanType::Progressive);
pin->setStreamInfo(vsi.get());
inputSocket->pins()->add(pin.get());

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Push frames manually
auto sample = primo::make_ref(Library::createMediaSample());

for (int frameIndex = 0; frameIndex < totalFrames; frameIndex++) {
    // Read or generate frame data
    std::vector<uint8_t> frameData = getFrameData(frameIndex);
    
    auto buffer = primo::make_ref(Library::createMediaBuffer());
    buffer->attach(frameData.data(), frameData.size(), true);
    sample->setBuffer(buffer.get());
    
    // Set presentation timestamp (required)
    sample->setStartTime(frameIndex / 30.0); // For 30 fps
    
    if (!transcoder->push(0, sample.get())) {
        printError("Transcoder push", transcoder->error());
        break;
    }
}

// Flush remaining data
if (!transcoder->flush()) {
    printError("Transcoder flush", transcoder->error());
}

transcoder->close();
```

### Using Presets

AVBlocks provides presets for common encoding scenarios:

```cpp
// Create output with preset name
auto outputSocket = primo::make_ref(Library::createMediaSocket("mp4.h264.aac"));
outputSocket->setFile(primo::ustring("output.mp4"));

// Or use specific preset
auto outputSocket = primo::make_ref(Library::createMediaSocket("ipad.mp4.h264.576p"));
outputSocket->setFile(primo::ustring("output.mp4"));

// Add to transcoder
transcoder->outputs()->add(outputSocket.get());
```

Common presets:
- **MP4**: `mp4.h264.aac` - H.264 video + AAC audio
- **iPad**: `ipad.mp4.h264.576p`, `ipad.mp4.h264.720p`
- **iPhone**: `iphone.mp4.h264.480p`
- **Android**: `android-phone.mp4.h264.360p`, `android-phone.mp4.h264.720p`, `android-tablet.mp4.h264.720p`
- **DVD**: `dvd.pal.4x3.mp2`, `dvd.ntsc.16x9.mp2`
- **WebM**: `webm.vp8.vorbis`, `android-tablet.webm.vp8.720p`
- **Apple TV**: `appletv.h264.480p`, `appletv.h264.720p`
- **VCD**: `vcd.pal`, `vcd.ntsc`

### Decoding Audio (Run Mode)

Decode AAC ADTS to raw PCM WAV:

```cpp
// Create input socket
auto inputSocket = primo::make_ref(Library::createMediaSocket());
inputSocket->setFile(primo::ustring("input.aac"));

// Create output socket for WAV
auto outputSocket = primo::make_ref(Library::createMediaSocket());
outputSocket->setFile(primo::ustring("output.wav"));
outputSocket->setStreamType(StreamType::WAVE);

auto pin = primo::make_ref(Library::createMediaPin());
auto asi = primo::make_ref(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::LPCM);
pin->setStreamInfo(asi.get());
outputSocket->pins()->add(pin.get());

// Setup and run transcoder
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (transcoder->open()) {
    transcoder->run();
    transcoder->close();
}
```

### Decoding Audio (Pull Mode)

Decode AAC and extract decoded PCM samples:

```cpp
// Setup input and output sockets
// ...

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Pull decoded audio samples
int32_t outputIndex = 0;
auto sample = primo::make_ref(Library::createMediaSample());

while (transcoder->pull(outputIndex, sample.get())) {
    // Process decoded audio
    outfile.write((const char*)sample->buffer()->data(),
                  sample->buffer()->dataSize());
}

const primo::error::ErrorInfo* error = transcoder->error();
if (error->facility() == primo::error::ErrorFacility::Codec &&
    error->code() == primo::codecs::CodecError::EOS) {
    // Success
}

transcoder->close();
```

### Encoding Audio (Run Mode)

Encode WAV file to MP3:

```cpp
// Input: WAV file
auto inputSocket = primo::make_ref(Library::createMediaSocket());
inputSocket->setFile(primo::ustring("input.wav"));

// Output: MP3
auto outputSocket = primo::make_ref(Library::createMediaSocket());
outputSocket->setFile(primo::ustring("output.mp3"));
outputSocket->setStreamType(StreamType::MPEG_Audio);
outputSocket->setStreamSubType(StreamSubType::MPEG_Audio_Layer3);

auto pin = primo::make_ref(Library::createMediaPin());
auto asi = primo::make_ref(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::MPEG_Audio);
asi->setStreamSubType(StreamSubType::MPEG_Audio_Layer3);
// Optional: set bitrate (default is 128000)
// asi->setBitrate(192000);
pin->setStreamInfo(asi.get());
outputSocket->pins()->add(pin.get());

// Setup and run transcoder
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (transcoder->open()) {
    transcoder->run();
    transcoder->close();
}
```

### Encoding Audio (Pull Mode)

Encode WAV to MP3 using pull mode:

```cpp
// Setup input and output sockets
// ...

std::ofstream outfile("output.mp3", std::ios::binary);

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Pull encoded samples
int32_t outputIndex = 0;
auto sample = primo::make_ref(Library::createMediaSample());

while (transcoder->pull(outputIndex, sample.get())) {
    outfile.write((const char*)sample->buffer()->data(),
                  sample->buffer()->dataSize());
}

const primo::error::ErrorInfo* error = transcoder->error();
if (error->facility() == primo::error::ErrorFacility::Codec &&
    error->code() == primo::codecs::CodecError::EOS) {
    // Success
}

transcoder->close();
```

### Encoding Audio (Push Mode)

Feed audio samples to encoder manually:

```cpp
// Output socket for AAC/MP3
// ...

// Input socket without file
auto inputSocket = primo::make_ref(Library::createMediaSocket());

auto pin = primo::make_ref(Library::createMediaPin());
auto asi = primo::make_ref(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::LPCM);
asi->setSampleRate(44100);
asi->setChannels(2);
asi->setBitsPerSample(16);
pin->setStreamInfo(asi.get());
inputSocket->pins()->add(pin.get());

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Push audio samples
auto sample = primo::make_ref(Library::createMediaSample());

// Read audio data in chunks
while (readAudioData(audioBuffer, bufferSize)) {
    auto buffer = primo::make_ref(Library::createMediaBuffer(bufferSize));
    memcpy(buffer->start(), audioBuffer, bufferSize);
    buffer->setData(0, bufferSize);
    
    sample->setBuffer(buffer.get());
    
    if (!transcoder->push(0, sample.get())) {
        break;
    }
}

transcoder->flush();
transcoder->close();
```

### Creating Video from Images

Create a video slideshow from a sequence of images:

```cpp
// Use MediaInfo to get properties of first image
auto info = primo::make_ref(Library::createMediaInfo());
std::string firstImage = inputDir + "/image_0000.jpeg";
info->inputs()->at(0)->setFile(primo::ustring(firstImage));

if (!info->open()) {
    return false;
}

// Create input socket without file (we'll push images)
auto inputSocket = primo::make_ref(Library::createMediaSocket());
auto pin = primo::make_ref(Library::createMediaPin());

// Clone video info from first image and set frame rate
auto vsi = primo::make_ref(
    (VideoStreamInfo*)info->outputs()->at(0)->pins()->at(0)->streamInfo()->clone()
);
vsi->setFrameRate(25.0); // 25 fps slideshow

pin->setStreamInfo(vsi.get());
inputSocket->pins()->add(pin.get());

// Create output socket with preset
auto outputSocket = primo::make_ref(Library::createMediaSocket("mp4.h264.aac"));
outputSocket->setFile(primo::ustring("output.mp4"));

auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Push images
auto sample = primo::make_ref(Library::createMediaSample());

for (int i = 0; i < imageCount; i++) {
    // Read image file into buffer
    std::string imgFile = inputDir + "/image_" + std::to_string(i) + ".jpeg";
    std::vector<uint8_t> imageData = readFileBytes(imgFile);
    
    auto buffer = primo::make_ref(Library::createMediaBuffer());
    buffer->attach(imageData.data(), imageData.size(), true);
    sample->setBuffer(buffer.get());
    
    // Set correct timestamp for video frame (required)
    sample->setStartTime(i / 25.0);
    
    if (!transcoder->push(0, sample.get())) {
        break;
    }
}

transcoder->flush();
transcoder->close();
```

### Demuxing MP4 Container

Extract audio and video streams from MP4 container:

```cpp
// Use MediaInfo to analyze input
auto info = primo::make_ref(Library::createMediaInfo());
info->inputs()->at(0)->setFile(primo::ustring("input.mp4"));

if (!info->open()) {
    return false;
}

// Find audio and video streams
int audioStreamIndex = -1;
int videoStreamIndex = -1;

MediaSocket* socket = info->outputs()->at(0);
for (int i = 0; i < socket->pins()->count(); i++) {
    MediaPin* pin = socket->pins()->at(i);
    if (pin->streamInfo()->mediaType() == MediaType::Audio) {
        audioStreamIndex = i;
    }
    else if (pin->streamInfo()->mediaType() == MediaType::Video) {
        videoStreamIndex = i;
    }
}

// Create input socket
auto inputSocket = primo::make_ref(Library::createMediaSocket(info.get()));

// Create separate output sockets for audio and video
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->inputs()->add(inputSocket.get());

if (videoStreamIndex >= 0) {
    auto videoOutput = primo::make_ref(Library::createMediaSocket());
    videoOutput->setFile(primo::ustring("video.mp4"));
    videoOutput->setStreamType(StreamType::MP4);
    
    auto videoPin = primo::make_ref(Library::createMediaPin());
    videoPin->setStreamInfo(
        socket->pins()->at(videoStreamIndex)->streamInfo()
    );
    videoOutput->pins()->add(videoPin.get());
    transcoder->outputs()->add(videoOutput.get());
}

if (audioStreamIndex >= 0) {
    auto audioOutput = primo::make_ref(Library::createMediaSocket());
    audioOutput->setFile(primo::ustring("audio.mp4"));
    audioOutput->setStreamType(StreamType::MP4);
    
    auto audioPin = primo::make_ref(Library::createMediaPin());
    audioPin->setStreamInfo(
        socket->pins()->at(audioStreamIndex)->streamInfo()
    );
    audioOutput->pins()->add(audioPin.get());
    transcoder->outputs()->add(audioOutput.get());
}

// Setup transcoder and process
transcoder->setAllowDemoMode(true);
if (transcoder->open()) {
    transcoder->run();
    transcoder->close();
}
```

### Muxing Audio and Video into MP4

Combine separate audio and video streams into single MP4:

```cpp
// Create first input for video
auto videoInput = primo::make_ref(Library::createMediaSocket());
videoInput->setFile(primo::ustring("video.mp4"));

// Create second input for audio
auto audioInput = primo::make_ref(Library::createMediaSocket());
audioInput->setFile(primo::ustring("audio.mp4"));

// Create output socket
auto outputSocket = primo::make_ref(Library::createMediaSocket());
outputSocket->setFile(primo::ustring("output.mp4"));
outputSocket->setStreamType(StreamType::MP4);

// Add both video and audio pins to output
auto videoPin = primo::make_ref(Library::createMediaPin());
auto vsi = primo::make_ref(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::H264);
videoPin->setStreamInfo(vsi.get());
outputSocket->pins()->add(videoPin.get());

auto audioPin = primo::make_ref(Library::createMediaPin());
auto asi = primo::make_ref(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::AAC);
audioPin->setStreamInfo(asi.get());
outputSocket->pins()->add(audioPin.get());

// Setup transcoder
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(videoInput.get());
transcoder->inputs()->add(audioInput.get());
transcoder->outputs()->add(outputSocket.get());

if (transcoder->open()) {
    transcoder->run();
    transcoder->close();
}
```

### Re-encoding Media

Take an MP4 input and re-encode the streams:

```cpp
// Analyze input
auto info = primo::make_ref(Library::createMediaInfo());
info->inputs()->at(0)->setFile(primo::ustring("input.mp4"));

if (!info->open()) {
    return false;
}

// Create input socket from MediaInfo
auto inputSocket = primo::make_ref(Library::createMediaSocket(info.get()));

// Create output socket with different encoding parameters
auto outputSocket = primo::make_ref(Library::createMediaSocket("mp4.h264.aac"));
outputSocket->setFile(primo::ustring("output.mp4"));

// Optionally customize encoding parameters
for (int i = 0; i < outputSocket->pins()->count(); i++) {
    MediaPin* pin = outputSocket->pins()->at(i);
    
    if (pin->streamInfo()->mediaType() == MediaType::Video) {
        VideoStreamInfo* vsi = static_cast<VideoStreamInfo*>(pin->streamInfo());
        vsi->setBitrate(2000000); // 2 Mbps
        vsi->setFrameRate(30.0);
    }
    else if (pin->streamInfo()->mediaType() == MediaType::Audio) {
        AudioStreamInfo* asi = static_cast<AudioStreamInfo*>(pin->streamInfo());
        asi->setBitrate(128000); // 128 kbps
    }
}

// Setup and run transcoder
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (transcoder->open()) {
    transcoder->run();
    transcoder->close();
}
```

### Decoding H.264 Access Units

Decode a stream of H.264 Access Units (AUs):

```cpp
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->setAllowDemoMode(true);

bool decoderInitialized = false;
int32_t index = 0;

for (int i = 0; ; i++) {
    char auFile[PATH_MAX];
    snprintf(auFile, PATH_MAX, "%s/au_%04d.h264", inputDir.c_str(), i);
    
    if (!decoderInitialized) {
        // Use MediaInfo on first AU to get stream properties
        auto info = primo::make_ref(Library::createMediaInfo());
        info->inputs()->at(0)->setFile(primo::ustring(auFile));
        
        if (!info->open()) {
            return false;
        }
        
        // Create input socket without file (will push data)
        auto inSocket = primo::make_ref(Library::createMediaSocket(info.get()));
        inSocket->setFile(NULL);
        inSocket->setStream(NULL);
        
        // Create output socket for YUV
        auto outSocket = primo::make_ref(Library::createMediaSocket());
        outSocket->setFile(primo::ustring("output.yuv"));
        outSocket->setStreamType(StreamType::UncompressedVideo);
        
        auto pin = primo::make_ref(Library::createMediaPin());
        auto vsi = primo::make_ref(Library::createVideoStreamInfo());
        vsi->setStreamType(StreamType::UncompressedVideo);
        vsi->setColorFormat(ColorFormat::YUV420);
        pin->setStreamInfo(vsi.get());
        outSocket->pins()->add(pin.get());
        
        transcoder->inputs()->add(inSocket.get());
        transcoder->outputs()->add(outSocket.get());
        
        if (!transcoder->open()) {
            return false;
        }
        
        decoderInitialized = true;
    }
    
    // Read AU data
    std::vector<uint8_t> auData = readFileBytes(auFile);
    if (auData.size() <= 0)
        break;
    
    // Push AU to decoder
    auto sample = primo::make_ref(Library::createMediaSample());
    auto buffer = primo::make_ref(Library::createMediaBuffer());
    buffer->attach(auData.data(), auData.size(), true);
    sample->setBuffer(buffer.get());
    
    if (!transcoder->push(index, sample.get())) {
        printError("Transcoder push", transcoder->error());
        return false;
    }
}

transcoder->flush();
transcoder->close();
```

## Error Handling

Always check return values and handle errors properly:

```cpp
auto transcoder = primo::make_ref(Library::createTranscoder());
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    const primo::error::ErrorInfo* e = transcoder->error();
    
    if (e->facility() == primo::error::ErrorFacility::Success) {
        std::cout << "Success" << std::endl;
    }
    else {
        if (e->message()) {
            std::cout << "Error: " << primo::ustring(e->message()) << std::endl;
        }
        
        std::cout << "Facility: " << e->facility() 
                  << ", Code: " << e->code();
        
        if (e->hint()) {
            std::cout << ", Hint: " << primo::ustring(e->hint());
        }
        std::cout << std::endl;
    }
    return false;
}

// For pull mode, check for end of stream
while (transcoder->pull(outputIndex, sample.get())) {
    // Process sample
}

const primo::error::ErrorInfo* error = transcoder->error();
if (error->facility() == primo::error::ErrorFacility::Codec &&
    error->code() == primo::codecs::CodecError::EOS) {
    // Successfully reached end of stream - this is normal
}
else {
    // Actual error occurred
    printError("Transcoder pull", error);
}
```

## Memory Management

AVBlocks uses reference counting. Use `primo::ref<>` or `primo::make_ref()` smart pointers for automatic memory management:

```cpp
// Using primo::ref with explicit constructor
primo::ref<Transcoder> transcoder(Library::createTranscoder());
// Automatically released when ref goes out of scope

// Using primo::make_ref (C++11 style)
auto socket = primo::make_ref(Library::createMediaSocket());

// Raw pointers are used when object ownership is managed elsewhere
MediaPin* pin = socket->pins()->at(0);
StreamInfo* si = pin->streamInfo();

// Don't delete raw pointers obtained from AVBlocks API
// They are managed by their container objects
```

## Platform-Specific Considerations

### macOS
- Link against `libAVBlocks.dylib`
- Use `primo::ustring` for file paths to handle Unicode correctly

### Linux
- Link against `libAVBlocks64.so`
- Ensure library path is in `LD_LIBRARY_PATH` or use `rpath`
- Use `primo::ustring` for file paths to handle Unicode correctly

### Windows
- Link against `AVBlocks64.lib` (64-bit) 
- `AVBlocks64.dll` DLL must be in PATH or application directory
- Use `/MT` or `/MTd` runtime library option

## Sample Code

Complete working examples are available in the `samples` directory:

- **Media Info**: [`info_stream_file`](./samples/darwin/info_stream_file), [`info_metadata_file`](./samples/darwin/info_metadata_file)
- **Decoding**: 
  - Audio: [`dec_aac_adts_file`](./samples/darwin/dec_aac_adts_file), [`dec_aac_adts_pull`](./samples/darwin/dec_aac_adts_pull), [`dec_mp3_file`](./samples/darwin/dec_mp3_file), [`dec_g711_alaw_file`](./samples/darwin/dec_g711_alaw_file), [`dec_g711_ulaw_file`](./samples/darwin/dec_g711_ulaw_file)
  - Video: [`dec_avc_file`](./samples/darwin/dec_avc_file), [`dec_avc_au`](./samples/darwin/dec_avc_au)
- **Encoding**: 
  - Audio: [`enc_aac_adts_file`](./samples/darwin/enc_aac_adts_file), [`enc_aac_adts_pull`](./samples/darwin/enc_aac_adts_pull), [`enc_aac_adts_push`](./samples/darwin/enc_aac_adts_push), [`enc_mp3_file`](./samples/darwin/enc_mp3_file), [`enc_mp3_pull`](./samples/darwin/enc_mp3_pull), [`enc_mp3_push`](./samples/darwin/enc_mp3_push), [`enc_g711_alaw_file`](./samples/darwin/enc_g711_alaw_file), [`enc_g711_ulaw_file`](./samples/darwin/enc_g711_ulaw_file)
  - Video: [`enc_avc_file`](./samples/darwin/enc_avc_file), [`enc_avc_pull`](./samples/darwin/enc_avc_pull), [`enc_preset_file`](./samples/darwin/enc_preset_file)
- **Muxing/Demuxing**: [`mux_mp4_file`](./samples/darwin/mux_mp4_file), [`demux_mp4_file`](./samples/darwin/demux_mp4_file)
- **Advanced**: [`slideshow`](./samples/darwin/slideshow), [`re-encode`](./samples/darwin/re-encode)

# Development

## macOS

### Download AVBlocks Core and Assets

See [Download Core and Assets on macOS](./docs/download-avblocks-core-and-assets-mac.md) 

### Setup

See [Setup for macOS](./docs/setup-mac.md)

### Build

See [Build on macOS](./docs/build-mac.md)

### Run

See [README](./samples/darwin/README.md) in the `samples` subdirectory. 

## Linux

### Download AVBlocks Core and Assets

See [Download Core and Assets on Linux](./docs/download-avblocks-core-and-assets-linux.md) 

### Setup

See [Setup for Linux](./docs/setup-linux.md)

### Build

See [Build on Linux](./docs/build-linux.md)

### Run

See [README](./samples/linux/README.md) in the `samples` subdirectory. 

## Windows

### Download AVBlocks Core and Assets

See [Download Core and Assets on Windows](./docs/download-avblocks-core-and-assets-windows.md) 

### Setup

See [Setup for Windows](./docs/setup-windows.md)

### Build

See [Build on Windows](./docs/build-windows.md)

### Run

See [README](./samples/windows/README.md) in the `samples` subdirectory. 

# How to obtain Commercial License

See [License Options](https://avblocks.com/license/) for details.

We offer discounts for:

- Competitive product
- Startup
- Educational institution
- Open source project
