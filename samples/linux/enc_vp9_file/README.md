## enc_vp9_file

Encode a raw YUV video file to VP9 format in an IVF container.

### Command Line

```sh
enc_vp9_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf>
```

###	Examples

List options:

```sh
./bin/x64/enc_vp9_file --help
enc_vp9_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf>
[--colors]
  -h,    --help
  -i,    --input   input YUV file
  -o,    --output  output IVF file
  -r,    --rate    input frame rate
  -f,    --frame   input frame sizes <width>x<height>
  -c,    --color   input color format. Use --colors to list all supported color formats
         --colors  list COLOR constants
```

List supported color formats:

```sh
./bin/x64/enc_vp9_file --colors
```

Encode the raw YUV file `./assets/vid/foreman_qcif.yuv` to VP9 format:

```sh
mkdir -p ./output/enc_vp9_file

./bin/x64/enc_vp9_file \
    --input ./assets/vid/foreman_qcif.yuv \
    --output ./output/enc_vp9_file/foreman_qcif.ivf \
    --rate 30 \
    --frame 176x144 \
    --color yuv420
```
