# TODO list

- Licence requirement about the program distribute it's sources

- try https://wiki.libsdl.org/SDL3/README-highdpi
- read https://github.com/TheSpydog/SDL_gpu_examples/blob/main/Examples/Latency.c (SDL_SetGPUAllowedFramesInFlight)


- Artifact naming with commit hash when it is not a build from a tagged commit ?
  - Update metadata.h on the fly ?

- linux build instructions for non-available prebuilt llss-build-deps
  - provide an helper to install pre-built from one repo to the other ?
  - explicit where is the pre-build repository base URL

- renderdoc: add some in-program annotations
  - https://renderdoc.org/docs/how/how_annotate_capture.html#application-provided-marker-regions

- render: make it in a SDR_LINEAR or HDR_LINEAR texture before blitting to SDR_SRGB swapchain

- make some sound+video+subtitle at right PTS times
  - https://github.com/libsdl-org/SDL/blob/main/examples/audio/04-multiple-streams/multiple-streams.c

- make NUT encoding to a pipe
  - https://github.com/leandromoreira/ffmpeg-libav-tutorial

- make some camera input (via SDL API, will RAM to VRAM but ok as a start point)
  - https://examples.libsdl.org/SDL3/camera/01-read-and-draw/

- document installation and usage of shadercross
```
cd ~/bin
$ ln -s /opt/SDL3_shadercross-3.0.0-linux-x64/bin/shadercross
```
