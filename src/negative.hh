#ifndef NEGATIVE_H
#define NEGATIVE_H
#include <opencv2/opencv.hpp>

void applyNegative(cv::Mat& frame) {
    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
            color = cv::Vec3b(255 - color[0], 255 - color[1], 255 - color[2]);
        }
    }
}

#endif