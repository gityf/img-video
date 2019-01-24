/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class EndPointMgr.
*/
#ifndef VIDEO_PUSH_ENDPOINTMGR_H
#define VIDEO_PUSH_ENDPOINTMGR_H

#include <list>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

struct SEndPointInfo {
    string addr;
    int port;
    int fd;
    int failTimes;

    struct sockaddr_in sockaddr;
};

class EndPointMgr {
public:
    EndPointMgr();
    ~EndPointMgr();

    bool addEndPoint(const string& addr, int port);

    int sendData(unsigned char *send_buf, size_t len_sendbuf);

private:
    list<SEndPointInfo> mEndpointList;
};
#endif //VIDEO_PUSH_ENDPOINTMGR_H
