//
// Created by wangyaofu on 2019/2/20.
//

#ifndef UNTITLED_IMAGEFILELOADER_H
#define UNTITLED_IMAGEFILELOADER_H

#include <opencv2/opencv.hpp>
#include "../common/filesys.h"
#include "../common/timemeasurer.h"

class ImageFileLoader {
public:
    ImageFileLoader() {}

    virtual ~ImageFileLoader() {}

    void init() {
        doInit();
    }

    virtual void doInit() = 0;

    void run(const char* path) {
        std::vector<std::string> imgList;

        common::FileSys::ListFiles(path, &imgList);
        string fileName = "";
        TimeMeasurer tm;

        for (int j = 0; j < imgList.size(); ++j) {
            tm.Reset();
            fileName = path;
            fileName += imgList[j];
            cv::Mat src = imread(fileName, cv::IMREAD_ANYCOLOR);
            if (src.empty()) {
                continue;
            }

            tm.Reset();
            handle(src);
            std::cout << "cost=" << tm.Elapsed() << std::endl;
            cv::imshow("res", src);
            tm.Reset();
            cv::waitKey(10);
            getchar();
        }

    }

    virtual void handle(cv::Mat& image) = 0;
};


#endif //UNTITLED_IMAGEFILELOADER_H
