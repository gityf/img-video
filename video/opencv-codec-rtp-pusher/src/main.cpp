/*
** Copyright (C) 2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of Main process.
*/

#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "stream-push/StreamPush.h"
#include "common/singleton.h"

using namespace cv;

const static int kFrameWidth = 1280;
const static int kFrameHeight = 720;
const static int kFrameChannel = 3;
const static int kFrameFPS = 25;

#define STREAM_PUSH_INS Singleton<StreamPush>::Instance()

int initPush(const char *addr, int port, std::string codec) {
    EndPointMgr *endPointMgr = new EndPointMgr();
    endPointMgr->addEndPoint(addr, port);

    RtpStream *rtpStream = new RtpStream();

    rtpStream->SetEndPointMgr(endPointMgr);
    if (codec.compare("265") == 0) {
        STREAM_PUSH_INS->useCodec(AV_CODEC_ID_H265);
    } else if (codec.compare("vp8") == 0) {
        STREAM_PUSH_INS->useCodec(AV_CODEC_ID_VP8);
    } else if (codec.compare("vp9") == 0) {
        STREAM_PUSH_INS->useCodec(AV_CODEC_ID_VP9);
    } else {
        STREAM_PUSH_INS->useCodec(AV_CODEC_ID_H264);
    }
    STREAM_PUSH_INS->init(kFrameWidth, kFrameHeight, kFrameChannel, kFrameFPS);
    STREAM_PUSH_INS->setRtpStream(rtpStream);
    STREAM_PUSH_INS->setRtpEnable(true);

    return 0;
}

void handleVideo(const char *pFileName) {
    Mat frame;
    cv::VideoCapture capture(pFileName);

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }
        STREAM_PUSH_INS->push(&frame);
        usleep(20000);
    }
}

void handleCameraVideo(int cameraId) {
    Mat frame;
    cv::VideoCapture capture(cameraId);
    capture.set(cv::CAP_PROP_FRAME_WIDTH, kFrameWidth);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, kFrameHeight);

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }
        //printf("w:%d,h:%d\n", frame.cols, frame.rows);
        STREAM_PUSH_INS->push(&frame);
        usleep(20000);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 4) {
        initPush(argv[1], atoi(argv[2]), argv[3]);
        handleCameraVideo(0);
    } else {
        printf("Usage: ip port 265/264/vp8/vp9\n");
    }
    return 0;
}
