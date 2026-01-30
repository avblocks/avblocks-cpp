## enc_vp8_file

The enc_vp8_file sample shows how to encode raw YUV video to VP8 video in IVF container.   

### Command Line

```powershell
./enc_vp8_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf>
```

###	Examples

List options:

```powershell
./bin/x64/enc_vp8_file --help
enc_vp8_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf>
[--colors]
  -h,    --help
  -i,    --input     input YUV file
  -o,    --output    output IVF file
  -r,    --rate      input frame rate
  -f,    --frame     input frame sizes <width>x<height>
  -c,    --color     input color format. Use --colors to list all supported color formats
         --colors    list COLOR constants
```

The following command encodes raw YUV video from `assets/vid/foreman_qcif.yuv` to `output/enc_vp8_file/foreman_qcif.ivf`:

```powershell
mkdir -Force -Path ./output/enc_vp8_file

./bin/x64/enc_vp8_file `
  --input ./assets/vid/foreman_qcif.yuv `
  --output ./output/enc_vp8_file/foreman_qcif.ivf `
  --frame 176x144 `
  --rate 30 `
  --color yuv420
```

