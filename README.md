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
    
    // Your code here
    
    // Shutdown AVBlocks
    primo::avblocks::Library::shutdown();
    
    return 0;
}
```

## Core Concepts

### MediaSocket

A `MediaSocket` represents an input or output stream with specific media parameters (codec, format, resolution, etc.).

### Transcoder

The `Transcoder` class performs media conversion between input and output sockets.

## Common Patterns

### Decoding Video

Decode H.264/AVC video to raw YUV format:

```cpp
#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>

using namespace primo::avblocks;
using namespace primo::codecs;

// Create input socket for H.264
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
inputSocket->setFile("input.h264");

// Create output socket for YUV
primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
outputSocket->setFile("output.yuv");

primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::UncompressedVideo);
vsi->setColorFormat(ColorFormat::YUV420);
outputSocket->pins()->add(Library::createMediaPin(vsi.get()));

// Create and configure transcoder
primo::ref<Transcoder> transcoder(Library::createTranscoder());
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    // Handle error
    return false;
}

// Process
while (transcoder->pull(out_buffer, out_buffer_size)) {
    // Handle output data
}

transcoder->close();
```

### Encoding Video

Encode raw YUV video to H.264/AVC:

```cpp
// Create input socket for YUV
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::UncompressedVideo);
vsi->setColorFormat(ColorFormat::YUV420);
vsi->setFrameWidth(1920);
vsi->setFrameHeight(1080);
vsi->setFrameRate(30.0);
inputSocket->pins()->add(Library::createMediaPin(vsi.get()));

// Create output socket for H.264
primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
outputSocket->setFile("output.h264");
outputSocket->setStreamType(StreamType::H264);

// Create and run transcoder
primo::ref<Transcoder> transcoder(Library::createTranscoder());
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    return false;
}

// Push input data
primo::ref<MediaSample> sample(Library::createMediaSample());
// ... configure sample with YUV data
transcoder->push(0, sample.get());

transcoder->flush();
transcoder->close();
```

### Using Presets

AVBlocks provides presets for common encoding scenarios:

```cpp
// Create output with preset
primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
outputSocket->setFile("output.mp4");

// Use preset for iPad resolution
primo::ref<Param> preset(Library::createParam());
preset->setName(Param::Preset::Video::iPad::H264_1024x768_30p);
outputSocket->params()->add(preset.get());

// Rest of transcoder setup...
```

Available preset categories:
- `Param::Preset::Video::iPad::*` - iPad optimized formats
- `Param::Preset::Video::iPhone::*` - iPhone optimized formats
- `Param::Preset::Video::Generic::*` - Generic video formats
- `Param::Preset::Audio::Generic::*` - Audio formats

### Decoding Audio

Decode AAC ADTS to raw PCM:

```cpp
// Input: AAC ADTS
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
inputSocket->setFile("input.aac");

// Output: PCM
primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
outputSocket->setFile("output.pcm");

primo::ref<AudioStreamInfo> asi(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::LPCM);
asi->setPcmFlags(PcmFlags::SignedInt);
asi->setSampleRate(44100);
asi->setChannels(2);
asi->setBitsPerSample(16);
outputSocket->pins()->add(Library::createMediaPin(asi.get()));

// Setup and run transcoder...
```

### Encoding Audio

Encode raw PCM to AAC ADTS:

```cpp
// Input: PCM
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
primo::ref<AudioStreamInfo> asi(Library::createAudioStreamInfo());
asi->setStreamType(StreamType::LPCM);
asi->setPcmFlags(PcmFlags::SignedInt);
asi->setSampleRate(44100);
asi->setChannels(2);
asi->setBitsPerSample(16);
inputSocket->pins()->add(Library::createMediaPin(asi.get()));

// Output: AAC ADTS
primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
outputSocket->setFile("output.aac");
outputSocket->setStreamType(StreamType::AAC);
outputSocket->setStreamSubType(StreamSubType::AacAdts);

// Setup and run transcoder...
```

### Creating Video from Images

Create a video slideshow from a sequence of images:

```cpp
// Input: Image sequence
primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
inputSocket->setFile("image_%04d.jpg");

primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
vsi->setStreamType(StreamType::UncompressedVideo);
vsi->setFrameRate(25.0); // 25 fps
inputSocket->pins()->add(Library::createMediaPin(vsi.get()));

// Output: H.264 video with preset
primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
outputSocket->setFile("output.mp4");

primo::ref<Param> preset(Library::createParam());
preset->setName(Param::Preset::Video::Generic::MP4::Base_H264_AAC);
outputSocket->params()->add(preset.get());

// Setup and run transcoder...
```

## Error Handling

Always check return values and handle errors:

```cpp
primo::ref<Transcoder> transcoder(Library::createTranscoder());
transcoder->inputs()->add(inputSocket.get());
transcoder->outputs()->add(outputSocket.get());

if (!transcoder->open()) {
    const primo::error::ErrorInfo* e = transcoder->error();
    std::cout << "Error: " << primo::ustring(e->message()) 
              << ", facility: " << e->facility()
              << ", code: " << e->code() << std::endl;
    return false;
}
```

## Memory Management

AVBlocks uses reference counting. Use `primo::ref<>` smart pointers:

```cpp
primo::ref<Transcoder> transcoder(Library::createTranscoder());
// Automatically released when ref goes out of scope
```

## Platform-Specific Considerations

### macOS
- Link against `libAVBlocks.dylib`
- Required frameworks: `CoreFoundation`, `AppKit` (or `ApplicationServices`)

### Linux
- Link against `libAVBlocks.so`
- Required libraries: `pthread`, `dl`

### Windows
- Link against `AVBlocks64.lib` (64-bit) or `AVBlocks.lib` (32-bit)
- DLLs must be in PATH or application directory

## Sample Code

Complete working examples are available in the `samples` directory:

- **Decoding**: [`dec_aac_adts_file`](./samples/darwin/dec_aac_adts_file), [`dec_avc_file`](./samples/darwin/dec_avc_file), [`dec_avc_au`](./samples/darwin/dec_avc_au)
- **Encoding**: [`enc_aac_adts_file`](./samples/darwin/enc_aac_adts_file), [`enc_avc_file`](./samples/darwin/enc_avc_file), [`enc_preset_file`](./samples/darwin/enc_preset_file)
- **Advanced**: [`slideshow`](./samples/darwin/slideshow), [`demux_mp4_file`](./samples/darwin/demux_mp4_file)

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
