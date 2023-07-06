#ifndef GRAY_SCALE_H
#define GRAY_SCALE_H
#include <opencv2/opencv.hpp>

inline void applyGrayscale(cv::Mat& frame) {
    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
            int grayscale = 0.2989 * color[2] + 0.5870 * color[1] + 0.1140 * color[0];
            color = cv::Vec3b(grayscale, grayscale, grayscale);
        }
    }
}

#endif