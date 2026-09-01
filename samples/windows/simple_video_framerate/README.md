## simple_video_framerate

Changes a video's frame rate from 24 fps to 30 fps using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks frame rate conversion workflow:

1. Initialize the AVBlocks library
2. Create a `MediaInfo` object to probe the input file
3. Create an input `MediaSocket` from the probed media info
4. Clone the input socket to start with an identical output configuration
5. Set the output frame rate to 30 fps on the output video pin
6. Configure a `Transcoder` with the input and output sockets
7. Run the transcode operation

No command line parsing is used — the input and output file paths are hardcoded in the source code.

### Running

1. Download the sample video, the MP4 file from the TED talk video [What's the next window into our universe?](https://archive.org/details/AndrewConnolly_2014) by Andrew Connolly (Wide 480p, 854x480, 24 fps):

    ```cmd
    curl -L -o AndrewConnolly_2014.mp4 "https://archive.org/download/AndrewConnolly_2014/AndrewConnolly_2014.mp4"
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```cmd
    bin\x64\simple_video_framerate.exe
    ```

3. The converted output file `AndrewConnolly_2014_30fps.mp4` will be created in the `output\simple_video_framerate` directory.

### Notes

- The input file `AndrewConnolly_2014.mp4` and output file `AndrewConnolly_2014_30fps.mp4` paths are hardcoded in `simple_video_framerate.cpp`. They are relative to the executable directory.
- If `transcoder->open()` fails, it may be because the output file already exists. Delete the output file and try again.
- The source video is encoded at 24 fps; the output video stream frame rate is set to 30 fps. The audio stream is copied from the source as is.
