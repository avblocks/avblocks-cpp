## enc_opus_file

The enc_opus_file sample shows how to encode a WAV file to Opus audio in OGG container.   

### Command Line

```powershell
./enc_opus_file --input <wav file> --output <opus file>
```

###	Examples

List options:

```powershell
./bin/x64/enc_opus_file --help
enc_opus_file --input <wav file> --output <opus file>
  -h,    --help
  -i,    --input     input WAV file
  -o,    --output    output Opus file
```

The following command encodes WAV audio from `assets/aud/equinox-48KHz.wav` to `output/enc_opus_file/equinox-48KHz.opus`:

```powershell
mkdir -Force -Path ./output/enc_opus_file

./bin/x64/enc_opus_file `
  --input ./assets/aud/equinox-48KHz.wav `
  --output ./output/enc_opus_file/equinox-48KHz.opus
```

