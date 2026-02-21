## dump_hevc_au

The `dump_hevc_au` sample shows how to split an H.265 (HEVC) elementary stream into access units (AU). The sample accepts an input H.265 file and an output folder. It reads the input file and writes each H.265 access unit to the output folder as a separate file. It also displays the NAL units within each access unit.

### Command Line

```powershell
./dump_hevc_au --input <h265-file> --output <folder>
```

or short form:

```powershell
./dump_hevc_au -i <h265-file> -o <folder>
```

### Examples

List options:

```powershell
./bin/x64/dump_hevc_au --help
Usage: dump_hevc_au --input <h265-file> --output <folder>
  -h,    --help
  -i,    --input    input file (HEVC/H.265)
  -o,    --output   output directory
```

The following command extracts the H.265 access units from the `foreman_qcif.h265` video and writes them to the folder `output/dump_hevc_au` as separate files (`au_####.h265`):

```powershell
mkdir -Force -Path ./output/dump_hevc_au

./bin/x64/dump_hevc_au `
  --input ./assets/vid/foreman_qcif.h265 `
  --output ./output/dump_hevc_au
```
