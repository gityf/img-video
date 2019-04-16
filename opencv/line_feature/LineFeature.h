//
// Created by wangyaofu on 2019/2/20.
//

#ifndef UNTITLED_LINEFEATURE_H
#define UNTITLED_LINEFEATURE_H

#include <opencv2/opencv.hpp>
#include "../fast_line_detector/fast_line_detector.hpp"
#include "ImageFileLoader.h"

class LineFeature : public ImageFileLoader {
public:
    LineFeature() {
        fastLineDetectorPtr_ = NULL;
    }
    ~LineFeature() {
        if (NULL != fastLineDetectorPtr_) {
            delete fastLineDetectorPtr_;
        }
    }

    virtual void doInit() {
            int    length_threshold    = 15;
            float  distance_threshold  = 1.41421356f;
            double canny_th1           = 50.0;
            double canny_th2           = 150.0;
            int    canny_aperture_size = 3;
            bool   do_merge            = true;
            fastLineDetectorPtr_ = cv::ximgproc::createRawFastLineDetector(length_threshold,
                                                                        distance_threshold,
                                                                        canny_th1,
                                                                        canny_th2,
                                                                        canny_aperture_size,
                                                                        do_merge);
    }

    virtual void handle(cv::Mat& rgbImage) {
        std::vector<cv::Vec4f> lines_fld;
        cv::Mat grayImage;
        cvtColor(image, grayImage, CV_BGR2GRAY);

        //gLineSegmentDetector->applyLSDetector(grayImage, lines);
        fastLineDetectorPtr_->detect(grayImage, lines_fld);


        for (int i = 0; i < lines_fld.size(); ++i) {
            cv::line(rgbImage,
                     cv::Point(lines_fld[i][0], lines_fld[i][1]),
                     cv::Point(lines_fld[i][2], lines_fld[i][3]),
                     cv::Scalar(0, 255, 0),
                     1, cv::LINE_AA);
        }
    }

    cv::ximgproc::FastLineDetector *fastLineDetectorPtr_;
};
#endif //UNTITLED_LINEFEATURE_H
