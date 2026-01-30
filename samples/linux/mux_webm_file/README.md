## mux_webm_file

Mux VP8 video and Vorbis audio elementary streams into a WebM container.

### Command Line

```sh
mux_webm_file --audio <input_vorbis.webm> --video <input_vp8.webm> --output <output.webm>
```

###	Examples

List options:

```sh
./bin/x64/mux_webm_file --help

Usage: mux_webm_file --audio <input_Vorbis> --video <input_VP8> --output <output.webm>
  -?,    --help
  -a,    --audio   input Vorbis files. Can be used multiple times
  -v,    --video   input VP8 files. Can be used multiple times
  -o,    --output  output file
```

Mux VP8 video and Vorbis audio into a WebM file:

```sh
mkdir -p ./output/mux_webm_file

./bin/x64/mux_webm_file \
    --audio ./assets/aud/big-buck-bunny_trailer_vp8_vorbis.aud.webm \
    --video ./assets/vid/big-buck-bunny_trailer_vp8_vorbis.vid.webm \
    --output ./output/mux_webm_file/big-buck-bunny_trailer.webm
```
