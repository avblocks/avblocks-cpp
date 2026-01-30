## demux_webm_file

The demux_webm_file sample shows how to demux a WebM file to separate audio and video WebM files.   

### Command Line

```powershell
./demux_webm_file --input <webm file> --output <output file name without extension>
```

###	Examples

List options:

```powershell
./bin/x64/demux_webm_file --help
Usage: demux_webm_file -i <input_webm_file> -o <output_webm_file_name_without_extension>
  -h,    --help
         --input    Input webm file.
         --output   Output webm filename (without extension).
```

The following command demuxes a WebM file from `assets/mov/big-buck-bunny_trailer_vp8_vorbis.webm`:

```powershell
mkdir -Force -Path ./output/demux_webm_file

./bin/x64/demux_webm_file `
  --input ./assets/mov/big-buck-bunny_trailer_vp8_vorbis.webm `
  --output ./output/demux_webm_file/big-buck-bunny_trailer_vp8_vorbis
```

