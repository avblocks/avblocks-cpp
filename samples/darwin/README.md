# Samples - C++ / macOS

## Media Info

### info_metadata_file

Extract metadata information from a media file.   

See [info_metadata_file](./info_metadata_file) for details.

### info_stream_file

Extract audio / video stream information from a media file.   

See [info_stream_file](./info_stream_file) for details.

---

## Demuxing

### MP4 

> MPEG-4 Part 14 / MPEG-4 Part 1

#### demux_mp4_file

Extract the first audio and video elementary stream from an MP4 container and save each stream into a separate MP4 file.

See [demux_mp4_file](./demux_mp4_file) for details.

---

## Muxing

### MP4 

> MPEG-4 Part 1

#### mux_mp4_file

Multiplex two single-stream MP4 files containing AAC (audio) and H.264 (video) streams into an MP4 (container) file.

See [mux_mp4_file](./demux_mp4_file) for details.

---

## Decoding

### AAC 

> Advanced Audio Coding

#### dec_aac_adts_file

Decode AAC file in Audio Data Transport Stream (ADTS) format and save the output to WAV file.

See [dec_aac_adts_file](./dec_aac_adts_file) for details.

#### dec_aac_adts_pull

Decode AAC file in Audio Data Transport Stream (ADTS) format using `Transcoder::pull` and save output to WAV file.

See [dec_aac_adts_pull](./dec_aac_adts_pull) for details.

### AVC / H.264

> Advanced Video Coding

#### dec_avc_au

Decode AVC / H.264 stream. The sample uses sequence of files to simulate a stream of H.264 Access Units (AUs) and a Transcoder object to decode the AUs to raw YUV video frames.    

See [dec_avc_au](./dec_avc_au) for details.

#### dec_avc_file

Decode a compressed AVC / H.264 file to raw uncompressed YUV video file.       

See [dec_avc_file](./dec_avc_file) for details.

### MP3

> MPEG-1 Layer 3 Audio

#### dec_mp3_file

Decode MP3 file and save output to WAV file.

See [dec_mp3_file](./dec_mp3_file) for details.

---

## Encoding

### AAC 

> Advanced Audio Coding

#### enc_aac_adts_file

Encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

See [enc_aac_adts_file](./enc_aac_adts_file) for details.

#### enc_aac_adts_pull

Encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format using `Transcoder::pull`.

See [enc_aac_adts_pull](./enc_aac_adts_pull) for details.

#### enc_aac_adts_push

Encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format using `Transcoder::push`.

See [enc_aac_adts_push](./enc_aac_adts_push) for details.

### AVC / H.264 

> Advanced Video Coding

#### enc_avc_file

Convert a raw YUV video file to a compressed AVC / H.264 video file.  

See [enc_avc_file](./enc_avc_file) for details.

### G.711

#### enc_g711_alaw_file

Encode WAV file to G.711 A-law WAV file.

See [enc_g711_alaw_file](./enc_g711_alaw_file) for details.

#### enc_g711_ulaw_file

Encode WAV file to G.711 μ-law WAV file.

See [enc_g711_ulaw_file](./enc_g711_ulaw_file) for details.


### MP3 

> MPEG-1 Layer 3 Audio

#### enc_mp3_file

Encode WAV file to MP3 file.

See [enc_mp3_file](./enc_mp3_file) for details.

#### enc_mp3_pull

Encode WAV file to MP3 file using `Transcoder::pull`.

See [enc_mp3_pull](./enc_mp3_pull) for details.

#### enc_mp3_push

How to encode WAV file to MP3 file using `Transcoder::push`.

See [enc_mp3_push](./enc_mp3_push) for details.

### Other 

#### enc_preset_file

Convert a raw YUV video file to a compressed video file. The format of the output is configured with an AVBlocks preset.

See [enc_preset_file](./enc_preset_file) for details.

---

## Misc

### re-encode

Take an MP4 input and re-encodes the audio and video streams back into an MP4 output. 

See [re-encode](./re-encode) for details.

### slideshow

Create a video from a sequence of images.

See [slideshow](./slideshow) for details.
