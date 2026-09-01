## dec_avc_pull

The dec_avc_pull sample shows how to use `Transcoder::pull` to decode an AVC/H.264 Annex B elementary stream to raw uncompressed YUV frames.

This sample demonstrates the pull-based decoding pattern where samples are explicitly requested from the transcoder in a loop, returning one decoded frame per call.

### Command Line

``` cmd
dec_avc_pull.exe --input <file> --output <yuv_file>
```

### Examples

List options:

```cmd
bin\x64\dec_avc_pull.exe --help
Usage: dec_avc_pull -i <h264 file> -o <yuv file>
  -h,    --help
         --input    H264 input file.
         --output   YUV output file.
```

The following command decodes an AVC/H.264 video from `assets/vid/foreman_qcif.h264` to `output/dec_avc_pull/foreman_qcif.yuv`:

```cmd
mkdir .\output\dec_avc_pull

bin\x64\dec_avc_pull.exe ^
    --input .\assets\vid\foreman_qcif.h264 ^
    --output .\output\dec_avc_pull\foreman_qcif.yuv
```
