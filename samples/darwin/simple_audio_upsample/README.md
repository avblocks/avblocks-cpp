# simple_audio_upsample

Upsamples an MP3 audio clip from 44.1 KHz to 48 KHz using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks audio upsampling workflow:

1. Initialize the AVBlocks library
2. Create a `MediaInfo` object to probe the input file
3. Create an input `MediaSocket` from the probed media info
4. Clone the input socket to start with an identical output configuration
5. Set the output sample rate to 48000 Hz on the output audio pin
6. Configure a `Transcoder` with the input and output sockets
7. Run the transcode operation

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```sh
mkdir -p build/debug_x64
cd build/debug_x64
cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=debug -DPLATFORM=x64 ../../samples
ninja
```

The built executable will be placed in `bin/x64/simple_audio_upsample`.

## Running

1. Download the sample audio, the `kahvi011_kennybeltrey-hydrate.mp3` song from the [Internet Archive](https://archive.org/details/kahvi011) (MPEG Audio Layer 3, 44.1 KHz, Joint Stereo):

    ```sh
    cd samples/darwin/simple_audio_upsample
    curl -L -o kahvi011_kennybeltrey-hydrate.mp3 \
        https://archive.org/download/kahvi011/kahvi011_kennybeltrey-hydrate.mp3
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```sh
    ../../../bin/x64/simple_audio_upsample
    ```

3. The upsampled output file `kahvi011_kennybeltrey-hydrate_48Khz.mp3` will be created in the `samples/darwin/simple_audio_upsample` directory.

## Notes

- The input file `kahvi011_kennybeltrey-hydrate.mp3` and output file `kahvi011_kennybeltrey-hydrate_48Khz.mp3` paths are hardcoded in `simple_audio_upsample.cpp`. They are relative to the working directory from which the executable is run.
- If `transcoder->open()` fails, it may be because `kahvi011_kennybeltrey-hydrate_48Khz.mp3` already exists in the directory. Delete the output file and try again.
- The output audio stream is resampled from 44.1 KHz to 48 KHz using AVBlocks' polyphase resampling method.
