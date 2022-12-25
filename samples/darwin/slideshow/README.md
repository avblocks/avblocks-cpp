## slideshow

Create a video clip from a sequence of images. The sample input is a series of JPEG images. The output is configured with an AVBlocks preset.

### Command Line

```sh
slideshow [--preset PRESET]
```


###	Examples

List options:

```sh
./bin/x64/slideshow --help

Usage: slideshow [--preset PRESET]
PRESETS
-----------------
dvd.pal.4x3.mp2                               .mpg
dvd.pal.16x9.mp2                              .mpg
dvd.ntsc.4x3.mp2                              .mpg
dvd.ntsc.16x9.mp2                             .mpg
appletv.h264.480p                             .mp4
appletv.h264.720p                             .mp4
appletv.mpeg4.480p                            .mp4
wifi.h264.640x480.30p.1200k.aac.96k           .ts
wifi.wide.h264.1280x720.30p.4500k.aac.128k    .ts
mp4.h264.aac                                  .mp4
ipad.mp4.h264.576p                            .mp4
ipad.mp4.h264.720p                            .mp4
ipad.mp4.mpeg4.480p                           .mp4
iphone.mp4.h264.480p                          .mp4
iphone.mp4.mpeg4.480p                         .mp4
ipod.mp4.h264.240p                            .mp4
ipod.mp4.mpeg4.240p                           .mp4
android-phone.mp4.h264.360p                   .mp4
android-phone.mp4.h264.720p                   .mp4
android-tablet.mp4.h264.720p                  .mp4
android-tablet.webm.vp8.720p                  .webm
vcd.pal                                       .mpg
vcd.ntsc                                      .mpg
webm.vp8.vorbis                               .webm
```

Create a MP4 / H.264 video from a sequence of images in the `assets/img` folder using the `mp4.h264.aac` preset:

```sh
mkdir -p ./output/slideshow

./bin/x64/slideshow --preset mp4.h264.aac
```
