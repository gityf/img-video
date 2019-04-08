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

/**
 * image white balance
 */
static void imageWhiteBalance(cv::Mat &rgbImage) {
    std::vector<cv::Mat> imageRGB;

    //RGB三通道分离
    cv::split(rgbImage, imageRGB);

    //求原始图像的RGB分量的均值
    double R, G, B;
    B = cv::mean(imageRGB[0])[0];
    G = cv::mean(imageRGB[1])[0];
    R = cv::mean(imageRGB[2])[0];

    //需要调整的RGB分量的增益
    double KR, KG, KB;
    KB = (R + G + B) / (3 * B);
    KG = (R + G + B) / (3 * G);
    KR = (R + G + B) / (3 * R);

    //调整RGB三个通道各自的值
    imageRGB[0] = imageRGB[0] * KB;
    imageRGB[1] = imageRGB[1] * KG;
    imageRGB[2] = imageRGB[2] * KR;

    //RGB三通道图像合并
    cv::merge(imageRGB, rgbImage);
}

/**
 * image color balance
 */
void imageColorBalance(cv::Mat &rgbImage) {
    std::vector<cv::Mat> imageRGB;

    //RGB三通道分离
    cv::split(rgbImage, imageRGB);

    int histR[256] = {0};
    int histG[256] = {0};
    int histB[256] = {0};

    int num_of_pixels = rgbImage.total();
    for (int y = 0; y < rgbImage.rows; ++y) {
        uchar *ptSrcRow = rgbImage.ptr<uchar>(y);
        for (int x = 0; x < rgbImage.cols; x += 3) {
            histB[ptSrcRow[x]] += 1;
            histG[ptSrcRow[x+1]] += 1;
            histR[ptSrcRow[x+2]] += 1;
        }
    }

    for (int i = 1; i < 256; ++i) {
        histB[i] = histB[i] + histB[i - 1];
        histG[i] = histG[i] + histG[i - 1];
        histR[i] = histR[i] + histR[i - 1];
    }

    double s = 0.1;

    int minBorder = cvRound(num_of_pixels * s / 2);
    int maxBorder = cvRound(num_of_pixels * (1- s)/2);

    int vmins[3] = {0};
    int vmaxs[3] = {0};
    vmins[0] = 0;
    vmins[1] = 0;
    vmins[2] = 0;
    vmaxs[0] = 254;
    vmaxs[1] = 254;
    vmaxs[2] = 254;

    while (histB[vmins[0] + 1] <= minBorder) {
        vmins[0] = vmins[0] + 1;
    }
    while (histG[vmins[1] + 1] <= minBorder) {
        vmins[1] = vmins[1] + 1;
    }
    while (histR[vmins[2] + 1] <= minBorder) {
        vmins[2] = vmins[2] + 1;
    }
    while (histB[vmaxs[0] - 1] > maxBorder) {
        vmaxs[0] = vmaxs[0] - 1;
    }
    while (histG[vmaxs[1] - 1] > maxBorder) {
        vmaxs[1] = vmaxs[1] - 1;
    }
    while (histR[vmaxs[2] - 1] > maxBorder) {
        vmaxs[2] = vmaxs[2] - 1;
    }
    for (int j = 0; j < 3; ++j) {
        if (vmaxs[j] < 254) {
            vmaxs[j] = vmaxs[j] + 1;
        }
    }

    for (int y = 0; y < rgbImage.rows; ++y) {
        uchar *ptSrcRow = rgbImage.ptr<uchar>(y);
        for (int x = 0; x < rgbImage.cols; x += 3) {
            if (ptSrcRow[x] < vmins[0]) {
                ptSrcRow[x] = vmins[0];
            } else if (ptSrcRow[x] > vmaxs[0]) {
                ptSrcRow[x] = vmaxs[0];
            }
            if (ptSrcRow[x+1] < vmins[1]) {
                ptSrcRow[x+1] = vmins[1];
            } else if (ptSrcRow[x+1] > vmaxs[1]) {
                ptSrcRow[x+1] = vmaxs[1];
            }
            if (ptSrcRow[x+2] < vmins[2]) {
                ptSrcRow[x+2] = vmins[2];
            } else if (ptSrcRow[x+2] > vmaxs[2]) {
                ptSrcRow[x+2] = vmaxs[2];
            }

            ptSrcRow[x]   = cvRound((double)(ptSrcRow[x] - vmins[0]) * 255.0 / (vmaxs[0] - vmins[0]));
            ptSrcRow[x+1] = cvRound((double)(ptSrcRow[x+1] - vmins[1]) * 255.0 / (vmaxs[1] - vmins[1]));
            ptSrcRow[x+2] = cvRound((double)(ptSrcRow[x+2] - vmins[2]) * 255.0 / (vmaxs[2] - vmins[2]));
        }
    }

    //RGB三通道图像合并
    cv::merge(imageRGB, rgbImage);
}