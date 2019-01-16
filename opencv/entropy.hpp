#include <opencv2/opencv.hpp>


using cv::Mat;

#define KMAX_GRAY_VALUE 256

/**
 * 计算图像信息熵
 * @param img
 * @return
 */
static double Entropy(Mat img) {
    double temp[KMAX_GRAY_VALUE] = {0.0};

    // 计算每个像素的累积值
    for (int m = 0; m < img.rows; m++) {
        const uchar *t = img.ptr<uchar>(m);
        for (int n = 0; n < img.cols; n++) {
            int i = t[n];
            temp[i] = temp[i] + 1;
        }
    }

    double result = 0;

    for (int i = 0; i < KMAX_GRAY_VALUE; i++) {
        // 计算每个像素的概率
        temp[i] = temp[i] / (img.rows * img.cols);
        // 计算图像信息熵
        result = (temp[i] == 0.0) ? result : result - temp[i] * (log(temp[i]) / log(2.0));
    }

    return result;

}

/**
 * 计算图像互信息熵
 * @param img1
 * @param img2
 * @param img1_entropy
 * @param img2_entropy
 * @return
 */
static double ComEntropy(Mat img1, Mat img2, double& img1_entropy, double& img2_entropy) {
    double temp[KMAX_GRAY_VALUE][KMAX_GRAY_VALUE] = {0.0};

    // 计算联合图像像素的累积值
    for (int m1 = 0, m2 = 0; m1 < img1.rows, m2 < img2.rows; m1++, m2++) {
        const uchar *t1 = img1.ptr<uchar>(m1);
        const uchar *t2 = img2.ptr<uchar>(m2);
        for (int n1 = 0, n2 = 0; n1 < img1.cols, n2 < img2.cols; n1++, n2++) {
            int i = t1[n1], j = t2[n2];
            temp[i][j] = temp[i][j] + 1;
        }
    }

    double result = 0.0;
    for (int i = 0; i < KMAX_GRAY_VALUE; i++) {
        for (int j = 0; j < KMAX_GRAY_VALUE; j++) {
            // 计算每个联合像素的概率
            temp[i][j] = temp[i][j] / (img1.rows * img1.cols);
            //计算图像联合信息熵
            result = (temp[i][j] == 0.0) ? result : result - temp[i][j] * (log(temp[i][j]) / log(2.0));
        }
    }

    //得到两幅图像的互信息熵
    img1_entropy = Entropy(img1);
    img2_entropy = Entropy(img2);
    result = img1_entropy + img2_entropy - result;

    return result;

}