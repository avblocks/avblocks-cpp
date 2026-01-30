## dec_opus_file

The dec_opus_file sample shows how to decode Opus audio to WAV uncompressed audio.   

### Command Line

```powershell
./dec_opus_file --input <opus file> --output <wav file>
```

###	Examples

List options:

```powershell
./bin/x64/dec_opus_file --help
dec_opus_file --input <opus file> --output <wav file>
  -h,    --help
         --input    input Opus file
         --output   output WAV file
```

The following command decodes Opus audio from `assets/aud/Everybody-TBB.opus` to `output/dec_opus_file/Everybody-TBB.wav`:

```powershell
mkdir -Force -Path ./output/dec_opus_file

./bin/x64/dec_opus_file `
  --input ./assets/aud/Everybody-TBB.opus `
  --output ./output/dec_opus_file/Everybody-TBB.wav
```

