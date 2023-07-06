#ifndef FISHEYE_FILTER_H
#define FISHEYE_FILTER_H

#include <opencv2/opencv.hpp>
#include <cmath>

void applyFisheye(cv::Mat& frame) {
    cv::Mat result = frame.clone();

    double w = frame.cols, h = frame.rows;
    double centerX = w / 2.0f, centerY = h / 2.0f;
    double strength = 10.0;
    double max_radius = std::sqrt(w * w + h * h) / 2.0;  // change here

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            double dx = x - centerX;
            double dy = y - centerY;
            double theta = std::atan2(dy, dx);
            double d = std::sqrt(dx * dx + dy * dy);
            double r = max_radius * sqrt(d / max_radius);  // change here

            int xS = centerX + r * cos(theta);
            int yS = centerY + r * sin(theta);

            if (xS >= 0 && xS < w && yS >= 0 && yS < h) {
                result.at<cv::Vec3b>(y, x) = frame.at<cv::Vec3b>(yS, xS);
            } else {
                result.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
        }
    }

    frame = result;
}

#endif
