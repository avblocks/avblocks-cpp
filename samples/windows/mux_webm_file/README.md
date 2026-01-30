## mux_webm_file

The mux_webm_file sample shows how to mux Vorbis audio and VP8 video into a WebM file.   

### Command Line

```powershell
./mux_webm_file --audio <input_Vorbis> --video <input_VP8> --output <output.webm>
```

###	Examples

List options:

```powershell
./bin/x64/mux_webm_file --help

Usage: mux_webm_file --audio <input_Vorbis> --video <input_VP8> --output <output.webm>
  -?,    --help
  -a,    --audio     input Vorbis files. Can be used multiple times
  -v,    --video     input VP8 files. Can be used multiple times
  -o,    --output    output file
```

The following command muxes Vorbis audio and VP8 video into `output/mux_webm_file/big-buck-bunny_trailer.webm`:

```powershell
mkdir -Force -Path ./output/mux_webm_file

./bin/x64/mux_webm_file `
  --audio ./assets/aud/big-buck-bunny_trailer_vp8_vorbis.aud.webm `
  --video ./assets/vid/big-buck-bunny_trailer_vp8_vorbis.vid.webm `
  --output ./output/mux_webm_file/big-buck-bunny_trailer.webm
```

