# simple_converter

Transcodes a media file from `H.264/AVC` to `H.265/HEVC` using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks transcoding workflow:

1. Initialize the AVBlocks library
2. Create a `MediaInfo` object to probe the input file
3. Create an input `MediaSocket` from the probed media info
4. Create an output `MediaSocket` using a preset (`MP4 / H.264 (AVC) + AAC`) and change the video stream type to `H.265 (HEVC)`
5. Configure a `Transcoder` with the input and output sockets
6. Run the transcode operation

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```sh
mkdir -p build/debug_x64
cd build/debug_x64
cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=debug -DPLATFORM=x64 ../../samples
ninja
```

The built executable will be placed in `bin/x64/simple_converter`.

## Running

1. Download the sample video:

    ```sh
    cd samples/darwin/simple_converter
    curl -L -o Wildlife_h264_aac.mp4 https://archive.org/download/WildlifeSampleVideo/Wildlife.mp4
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```sh
    ../../../bin/x64/simple_converter
    ```

3. The transcoded output file `Wildlife_h265_aac.mp4` will be created in the `samples/darwin/simple_converter` directory.

## Notes

- The input file `Wildlife_h264_aac.mp4` and output file `Wildlife_h265_aac.mp4` paths are hardcoded in `simple_converter.cpp`. They are relative to the working directory from which the executable is run.
- If `transcoder->open()` fails, it may be because `Wildlife_h265_aac.mp4` already exists in the directory. Delete the output file and try again.
- The output uses the `Preset::Video::Generic::MP4::Base_H264_AAC` preset with the video stream type changed to `StreamType::H265`, producing an MP4 container with H.265 video and AAC audio streams.
