## remux

Take an MP4 input and re-encodes the audio and video streams back into MP4 output. Shows how to force encoding of individual streams even when it is not needed. 

### Command Line

```sh
remux [--input <input_file.mp4>] [--output <output_file.mp4>] [--audio <yes|no>] [--video <yes|no>]
```

###	Examples

List options:

```sh
./bin/x64/remux --help

Usage: mux_mp4_file --audio <input_AAC> --video <input_AVC> --output <output.mp4>
  -?,    --help
  -a,    --audio    input AAC files. Can be used multiple times
  -v,    --video    input H264 files. Can be used multiple times
  -o,    --output   output file
```

Re-encode the video stream of an MP4 / H.264 clip.

```sh
mkdir -p ./output/remux

./bin/x64/remux \
  --audio yes \
  --video yes \
  --input ./assets/mov/big_buck_bunny_trailer.mp4 \
  --output ./output/remux/big_buck_bunny_trailer.mp4
```
