## dec_vorbis_file

The dec_vorbis_file sample shows how to decode Vorbis audio in OGG container to WAV uncompressed audio.   

### Command Line

```powershell
./dec_vorbis_file --input <ogg file> --output <wav file>
```

###	Examples

List options:

```powershell
./bin/x64/dec_vorbis_file --help
dec_vorbis_file --input <ogg file> --output <wav file>
  -h,    --help
         --input    input OGG file
         --output   output WAV file
```

The following command decodes Vorbis audio from `assets/aud/Hydrate-Kenny_Beltrey.ogg` to `output/dec_vorbis_file/Hydrate-Kenny_Beltrey.wav`:

```powershell
mkdir -Force -Path ./output/dec_vorbis_file

./bin/x64/dec_vorbis_file `
  --input ./assets/aud/Hydrate-Kenny_Beltrey.ogg `
  --output ./output/dec_vorbis_file/Hydrate-Kenny_Beltrey.wav
```

