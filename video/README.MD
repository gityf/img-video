# How to run
## building

### building opencv-codec-rtp-pusher
``` sh
cd opencv-codec-rtp-pusher && make
# video-pusher
```

### building ffmpeg-h264-sdl-view
``` sh
cd ffmpeg-h264-sdl-view && make
# h264-viewer
```

### building ffmpeg-codec-sdl-view
``` sh
cd ffmpeg-codec-sdl-view && make
# video-viewer
```

## running
### h264 viewer and pusher
```sh
cd ffmpeg-h264-sdl-view
./h264-viewer 9964 264

cd opencv-codec-rtp-pusher
./video-pusher 127.0.0.1 9964 264
```

### h265 viewer and pusher
```sh
cd ffmpeg-codec-sdl-view
./video-viewer 9965 265

cd opencv-codec-rtp-pusher
./video-pusher 127.0.0.1 9965 265
```

### vp8 viewer and pusher
```sh
cd ffmpeg-codec-sdl-view
./video-viewer 9965 vp8

cd opencv-codec-rtp-pusher
./video-pusher 127.0.0.1 9965 vp8
```

### vp9 viewer and pusher
```sh
cd ffmpeg-codec-sdl-view
./video-viewer 9965 vp9

cd opencv-codec-rtp-pusher
./video-pusher 127.0.0.1 9965 vp9
```


## How it works?
opencv.VideoCapture --> x264/x265/vp8/vp9 encode --> rtp split slice --> udp --> rtp merge splice --> ffmpeg video decode --> sdl2 view show.

- ffmpeg video decode action will check whether x264/x265/vp8/vp9 encode action is ok or not

