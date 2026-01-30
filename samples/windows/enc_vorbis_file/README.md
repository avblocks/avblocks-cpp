## enc_vorbis_file

The enc_vorbis_file sample shows how to encode a WAV file to Vorbis audio in OGG container.   

### Command Line

```powershell
./enc_vorbis_file --input <wav file> --output <ogg file>
```

###	Examples

List options:

```powershell
./bin/x64/enc_vorbis_file --help
enc_vorbis_file --input <wav file> --output <ogg file>
  -h,    --help
  -i,    --input     input WAV file
  -o,    --output    output OGG file
```

The following command encodes WAV audio from `assets/aud/equinox-48KHz.wav` to `output/enc_vorbis_file/equinox-48KHz.ogg`:

```powershell
mkdir -Force -Path ./output/enc_vorbis_file

./bin/x64/enc_vorbis_file `
  --input ./assets/aud/equinox-48KHz.wav `
  --output ./output/enc_vorbis_file/equinox-48KHz.ogg
```

