//
// Created by wangyaofu on 2019/4/2.
//

#progma once

#include <opencv2/opencv.hpp>
#include <vector>

namespace xman {
    class Mat2Img {

    public:
        Mat2Img() {}

        ~Mat2Img() {}

    public:
        bool MatToByteArray(const cv::Mat &rgbImage, std::vector<unsigned char> &buff, int quality = 50) {
            if (rgbImage.empty()) {
                return false;
            }
            std::vector<int> param = std::vector<int>(2);
            param[0] = CV_IMWRITE_JPEG_QUALITY;
            param[1] = quality; // default(95) 0-100
            cv::imencode(".jpg", rgbImage, buff, param);
            return true;
        }

        bool JPEGToMat(cv::Mat &rgbImage, std::vector<unsigned char> &buff) {
            if (buff.empty()) {
                return false;
            }
            if ((buff[0] == 0xFF && buff[1] == 0xD8)) {
                rgbImage = cv::imdecode(buff, CV_LOAD_IMAGE_COLOR);
            } else {
                buff.insert(buff.begin(), 0xFF);
                buff.insert(buff.begin() + 1, 0xD8);
                rgbImage = cv::imdecode(buff, CV_LOAD_IMAGE_COLOR);
            }
            return true;
        }

        bool getJpgEncodeMat(cv::Mat &rgbImage, int quality = 50) {
            std::vector<unsigned char> buff;
            bool ret = MatToByteArray(rgbImage, buff, quality);
            return JPEGToMat(rgbImage, buff) && ret;
        }
    };
}
