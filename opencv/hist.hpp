//
// Created by wangyaofu
//

#ifndef DEMO1_HIST_H
#define DEMO1_HIST_H

#include <opencv2/opencv.hpp>

class Hist {
public:
    Hist() {}
    virtual ~Hist() {}

public:
    void CalcHist(cv::Mat &rgbImage) {
        /// 设定bin数目
        int histSize = 255;
        int histValues[3][255] = {};
        int histNormalizeValues[3][255] = {};
        for (int k = 0; k < histSize; ++k) {
            histValues[0][k] = 0;
            histValues[1][k] = 0;
            histValues[2][k] = 0;
            histNormalizeValues[0][k] = 0;
            histNormalizeValues[1][k] = 0;
            histNormalizeValues[2][k] = 0;
        }

        cv::Vec3b rgbPixel;
        // 遍历图像，统计BGR三个通道的图像的灰度值出现的次数
        for (int i = 0; i < rgbImage.rows; ++i) {
            for (int j = 0; j < rgbImage.cols; ++j) {
                // B G R
                rgbPixel = rgbImage.at<cv::Vec3b>(i, j);
                histValues[2][rgbPixel[2]] += 1;
                histValues[1][rgbPixel[1]] += 1;
                histValues[0][rgbPixel[0]] += 1;
            }
        }

        // 把如上的统计值归一化到0-255范围内
        calcNormalize(histValues[0], histNormalizeValues[0]);
        calcNormalize(histValues[1], histNormalizeValues[1]);
        calcNormalize(histValues[2], histNormalizeValues[2]);


        // 创建直方图画布
        int hist_w = 400; int hist_h = 400;
        int bin_w = cvRound( (double) hist_w/histSize );

        cv::Mat histImage( hist_w, hist_h, CV_8UC3, cv::Scalar( 255,255,255) );
        // 把三个通道的直方图归一化数据绘制在直方图上
        for (int i = 1; i < histSize; ++i) {
            cv::line(histImage,
                    cv::Point(bin_w * (i-1), hist_h - cvRound(histNormalizeValues[0][i-1])),
                    cv::Point(bin_w * (i), hist_h - cvRound(histNormalizeValues[0][i])),
                    cv::Scalar(0, 0, 255), 2,cv::LINE_AA, 0);
            cv::line(histImage,
                    cv::Point(bin_w * (i-1), hist_h - cvRound(histNormalizeValues[1][i-1])),
                    cv::Point(bin_w * (i), hist_h - cvRound(histNormalizeValues[1][i])),
                    cv::Scalar(0, 255, 0), 2,cv::LINE_AA, 0);
            cv::line(histImage,
                    cv::Point(bin_w * (i-1), hist_h - cvRound(histNormalizeValues[2][i-1])),
                    cv::Point(bin_w * (i), hist_h - cvRound(histNormalizeValues[2][i])),
                    cv::Scalar(255, 0, 0), 2,cv::LINE_AA, 0);
        }
        cv::imshow("histImage", histImage);
    }

    /**
     * 计算一个数组的归一化，此处归一化到0-255之间
     * @param srcValues
     * @param dstValues
     */
    void calcNormalize(int srcValues[255], int dstValues[255]) {
        int minValue = srcValues[0];
        int maxValue = srcValues[0];

        for (int i = 1; i < 255; ++i) {
            if (minValue > srcValues[i]) {
                minValue = srcValues[i];
            }
            if (maxValue < srcValues[i]) {
                maxValue = srcValues[i];
            }
        }
        int minMaxDiff = maxValue - minValue;
        for (int j = 0; j < 255; ++j) {
            dstValues[j] = static_cast<int>((float)(srcValues[j] - minValue) / (float)minMaxDiff * 255.);
        }
    }

    void EqualizeHist(cv::Mat &rgbImage) {
        std::vector<cv::Mat> rgbImages;
        cv::split(rgbImage, rgbImages);
        /// 应用直方图均衡化

        cv::Mat dstR, dstG, dstB;
        equalizeHist(rgbImages[0], dstB);
        equalizeHist(rgbImages[1], dstG);
        equalizeHist(rgbImages[2], dstR);

        std::vector<cv::Mat> grayHistImages;
        grayHistImages.push_back(dstB);
        grayHistImages.push_back(dstG);
        grayHistImages.push_back(dstR);
        cv::merge(grayHistImages, rgbImage);
    }

};

/**
* test code.
void HistShow() {
    cv::Mat rawImage = cv::imread("demo1/leopard2.png", cv::IMREAD_ANYCOLOR);
    cv::imshow("rawImage", rawImage);
    Hist hist;
    hist.CalcHist(rawImage);
    hist.EqualizeHist(rawImage);
    cv::imshow("EqualizeHistImage", rawImage);
    //hist.CalcHist(rawImage);
    cv::waitKey(-1);
    return;
}
*/
#endif //DEMO1_HIST_H
