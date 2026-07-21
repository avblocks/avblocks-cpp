# simple_video_crop

Crops a 16:9 video to 4:3 using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks video cropping workflow:

1. Initialize the AVBlocks library
2. Create a `MediaInfo` object to probe the input file
3. Create an input `MediaSocket` from the probed media info
4. Clone the input socket to start with an identical output configuration
5. Set the output frame size to 640x480 and the display ratio to 4:3
6. Set `Crop.Left` / `Crop.Right` parameters on the output video pin to cut the 854x480 source down to 640x480
7. Configure a `Transcoder` with the input and output sockets
8. Run the transcode operation

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```sh
mkdir -p build/debug_x64
cd build/debug_x64
cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=debug -DPLATFORM=x64 ../../samples
ninja
```

The built executable will be placed in `bin/x64/simple_video_crop`.

## Running

1. Download the sample video, the MP4 file from the TED talk video [What's the next window into our universe?](https://archive.org/details/AndrewConnolly_2014) by Andrew Connolly (Wide 480p, 854x480, 16:9):

    ```sh
    curl -L -o AndrewConnolly_2014.mp4 "https://archive.org/download/AndrewConnolly_2014/AndrewConnolly_2014.mp4"
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```sh
    cd samples/linux/simple_video_crop
    ../../../bin/x64/simple_video_crop
    ```

3. The cropped output file `AndrewConnolly_2014.mp4` will be created in the `samples/linux/simple_video_crop` directory.

## Notes

- The input file `AndrewConnolly_2014.mp4` and output file `AndrewConnolly_2014.mp4` paths are hardcoded in `simple_video_crop.cpp`. They are relative to the working directory from which the executable is run.
- If `transcoder->open()` fails, it may be because `AndrewConnolly_2014.mp4` already exists in the directory. Delete the output file and try again.
- The source video is 16:9 at 854x480. To crop it to 4:3 at 640x480, (854 - 640) / 2 = 107 pixels are cropped from the left and right edges. The audio stream is copied from the source as is.
