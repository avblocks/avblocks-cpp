## enc_opus_file

Encode a WAV file to Opus audio format in an OGG container.

### Command Line

```sh
enc_opus_file --input <wav_file> --output <opus_file>
```

###	Examples

List options:

```sh
./bin/x64/enc_opus_file --help
enc_opus_file --input <wav file> --output <opus file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output Opus file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` to Opus audio format:

```sh
mkdir -p ./output/enc_opus_file

./bin/x64/enc_opus_file \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_opus_file/equinox-48KHz.opus
```
