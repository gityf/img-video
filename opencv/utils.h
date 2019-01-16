#progma once

#include <opencv2/opencv.hpp>

/**
 * rect.* from type double to int.
 */
static void Rect2dToRect(cv::Rect2d &src, cv::Rect &dst) {
    dst.x = cvRound(src.x);
    dst.y = cvRound(src.y);
    dst.width = cvRound(src.width);
    dst.height = cvRound(src.height);
}

/**
 * rect.* from type int to double.
 */
static void RectToRect2d(cv::Rect &src, cv::Rect2d &dst) {
    dst.x = cvRound(src.x);
    dst.y = cvRound(src.y);
    dst.width = cvRound(src.width);
    dst.height = cvRound(src.height);
}

static void Rect2dToRectList(std::vector<cv::Rect2d> &srcList, std::vector<cv::Rect> &dstList) {
    for (int i = 0; i < srcList.size(); ++i) {
        cv::Rect rect;
        Rect2dToRect(srcList[i], rect);
        dstList.push_back(rect);
    }
}

static void RectToRect2dList(std::vector<cv::Rect> &srcList, std::vector<cv::Rect2d> &dstList) {
    for (int i = 0; i < srcList.size(); ++i) {
        cv::Rect2d rect;
        RectToRect2d(srcList[i], rect);
        dstList.push_back(rect);
    }
}
