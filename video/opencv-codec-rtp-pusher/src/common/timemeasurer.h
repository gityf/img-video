/*
** Copyright (C) 2014 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class TimeMeasurer.
*/
#ifndef _COMMON_CTIMEMEASURER_H_
#define _COMMON_CTIMEMEASURER_H_

#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
class TimeMeasurer {
public:
    TimeMeasurer() {
        Reset();
    }
    void Reset() {
        mLastCall = NowMs();
    }
    virtual ~TimeMeasurer() { }

    long Elapsed() {
        return NowMs() - mLastCall;
    }
    long lastMs() {
        return mLastCall;
    }
    long NowMs() {
        struct timeval timeNow;
        gettimeofday(&timeNow, NULL);
        return (timeNow.tv_sec) * 1000 + timeNow.tv_usec / 1000;
    }

private:
    long mLastCall;
};

#endif // _COMMON_CTIMEMEASURER_H_
