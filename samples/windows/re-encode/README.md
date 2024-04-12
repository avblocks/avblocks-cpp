## re-encode

This sample takes an MP4 input and re-encodes the audio and video streams back into MP4 output. It shows how to force encoding of individual streams even when it is not needed. 

### Command Line

```powershell
re-encode --input input_file.mp4 --output output_file.mp4 --audio <yes|no> --video <yes|no>
```

###	Examples

List options:

```powershell
./bin/x64/re-encode --help

Usage: re-encode --input inputFile.mp4 --output outputFile.mp4 [--audio yes|no] [--video yes|no]

  -h,    --help
  -i,    --input    input mp4 file
  -o,    --output   output mp4 file
  -a,    --audio    re-encode audio, yes|no
  -v,    --video    re-encode video, yes|no
```

The following example re-encodes only the video stream in an MP4 / H.264 clip: 

```powershell
mkdir -Force -Path ./output/re-encode

./bin/x64/re-encode `
    --input ./assets/mov/big_buck_bunny_trailer.mp4 `
    --output ./output/re-encode/big_buck_bunny_trailer.mp4 `
    --video yes `
    --audio no
```
