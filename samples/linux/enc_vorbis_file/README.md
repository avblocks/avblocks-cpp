## enc_vorbis_file

Encode a WAV file to Vorbis audio format in an OGG container.

### Command Line

```sh
enc_vorbis_file --input <wav_file> --output <ogg_file>
```

###	Examples

List options:

```sh
./bin/x64/enc_vorbis_file --help
enc_vorbis_file --input <wav file> --output <ogg file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output OGG file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` to Vorbis audio format:

```sh
mkdir -p ./output/enc_vorbis_file

./bin/x64/enc_vorbis_file \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_vorbis_file/equinox-48KHz.ogg
```
