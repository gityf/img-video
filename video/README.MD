# How to run
## building

### building opencv-x264-rtp-pusher
``` sh
cd opencv-x264-rtp-pusher && make
# video-pusher
```

### building ffmpeg-h264-sdl-view
``` sh
cd ffmpeg-h264-sdl-view && make
# viewer
```

### building ffmpeg-h265-sdl-view
``` sh
cd ffmpeg-h265-sdl-view && make
# x265-viewer
```

## running
### h264 viewer and pusher
```sh
cd ffmpeg-h264-sdl-view
./viewer 9964 264

cd opencv-x264-rtp-pusher
./video-pusher 127.0.0.1 9964 264
```

### h265 viewer and pusher
```sh
cd ffmpeg-h265-sdl-view
./viewer 9965 265

cd opencv-x264-rtp-pusher
./video-pusher 127.0.0.1 9965 265
```

## How it works?
opencv.VideoCapture --> x264/x265 encode --> rtp split slice --> udp --> rtp merge splice --> ffmpeg video decode --> sdl2 view show.

- ffmpeg video decode action will check whether x264/x265 encode action is ok or not
