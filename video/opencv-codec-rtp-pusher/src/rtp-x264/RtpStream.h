/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class RtpStream.
*/
#ifndef VIDEO_PUSH_RTPSTREAM_H
#define VIDEO_PUSH_RTPSTREAM_H

#include "EndPointMgr.h"

class RtpStream {
public:
    RtpStream();
    ~RtpStream();

    void SetEndPointMgr(EndPointMgr* endPointMgr);

    EndPointMgr* getEndPointMgr() {
        return mEndPointMgr;
    }

    int SendDataToEveryEdge(unsigned char *send_buf, size_t len_sendbuf);

    int H264Nal2RtpSend(int framerate, unsigned char *pstStream, size_t nalu_len);

private:
    EndPointMgr* mEndPointMgr;
};
#endif //VIDEO_PUSH_RTPSTREAM_H
