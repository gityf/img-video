//
// Created by wangyaofu on 2018/5/25.
//

#ifndef VIDEO_PUSH_STR_H
#define VIDEO_PUSH_STR_H

class Str {
public:
    Str() {
        data = NULL;
        size = 0;
    }
    void *data;
    size_t size;
};
#endif //VIDEO_PUSH_STR_H
