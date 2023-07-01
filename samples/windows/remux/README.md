## remux

This sample takes an MP4 input and re-encodes the audio and video streams back into MP4 output. It shows how to force encoding of individual streams even when it is not needed. 

### Command Line

```powershell
remux --input input_file.mp4 --output output_file.mp4 --remux-audio <yes|no> --remux-video <yes|no>
```

###	Examples

List options:

```powershell
./bin/x64/remux --help

Usage: remux [--input inputFile.mp4] [--output outputFile.mp4] [--remux-audio yes|no] [--remux-video yes|no]

  -?,    --help
  -i,    --input             input file
  -o,    --output            output file
  -a,    --remux-audio   remux audio with yes|no
  -v,    --remux-video   remux video with yes|no
```

The following example re-encodes only the video stream in an MP4 / H.264 clip: 

```powershell
mkdir -Force -Path ./output/remux

./bin/x64/remux `
    --input ./assets/mov/big_buck_bunny_trailer.mp4 `
    --output ./output/remux/big_buck_bunny_trailer.mp4 `
    --remux-video yes `
    --remux-audio no
```
