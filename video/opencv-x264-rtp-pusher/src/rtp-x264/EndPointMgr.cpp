/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class EndPointMgr.
*/
#include "EndPointMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

EndPointMgr::EndPointMgr() {

}

EndPointMgr::~EndPointMgr() {
    mEndpointList.clear();
}

bool EndPointMgr::addEndPoint(const string& addr, int port) {
    SEndPointInfo clientInfo;
    clientInfo.addr = addr;
    clientInfo.failTimes = 0;
    clientInfo.fd = 0;
    clientInfo.port = port;

    clientInfo.sockaddr.sin_family = AF_INET;
    clientInfo.sockaddr.sin_port = htons(port);
    clientInfo.sockaddr.sin_addr.s_addr = inet_addr(addr.data());
    clientInfo.fd = socket(AF_INET, SOCK_DGRAM, 0);

    mEndpointList.push_back(clientInfo);

    return true;
}

int EndPointMgr::sendData(unsigned char *send_buf, size_t len_sendbuf) {
    auto itor = mEndpointList.begin();
    int ret = 0;
    while(itor != mEndpointList.end()) {
        if ((ret = sendto(itor->fd, send_buf, len_sendbuf, 0, (struct sockaddr *)&itor->sockaddr, sizeof(itor->sockaddr))) < 0) {
            itor->failTimes++;
        }
        ++itor;
    }
    return ret;
}