/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of Main process.
*/

#include <opencv2/opencv.hpp>
#include "stream-push/StreamPush.h"
#include "common/singleton.h"

using namespace cv;

const static int kFrameWidth = 1280;
const static int kFrameHeight = 720;
const static int kFrameChannel = 3;
const static int kFrameFPS = 25;

#define STREAM_PUSH_INS Singleton<StreamPush>::Instance()

int initPush(const char* addr, int port) {
    EndPointMgr *endPointMgr = new EndPointMgr();
    endPointMgr->addEndPoint(addr, port);

    RtpStream* rtpStream = new RtpStream();

    rtpStream->SetEndPointMgr(endPointMgr);
    STREAM_PUSH_INS->init(kFrameWidth, kFrameHeight, kFrameChannel, kFrameFPS);
    STREAM_PUSH_INS->setRtpStream(rtpStream);
    STREAM_PUSH_INS->setRtpEnable(true);
    return 0;
}

void handleVideo(const char* pFileName) {
    Mat frame;
    cv::VideoCapture capture(pFileName);

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }

        STREAM_PUSH_INS->push(&frame);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 4) {
        initPush(argv[1], atoi(argv[2]));
        handleVideo(argv[3]);
    } else {
        printf("Usage: ip sport video-file-name\n");
    }
    return 0;
}
