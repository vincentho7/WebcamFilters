#ifndef BGBLUR_H
#define BGBLUR_H
#include <opencv2/opencv.hpp>
#include <vector>

void applyOtsuThresholdAndBlur(cv::Mat& frame) {
    /*
    cv::Mat result = frame.clone();

    // 1. Create a 5x5 Gaussian blur kernel
    double kernel[5][5] = {
        {1,  4,  6,  4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1,  4,  6,  4, 1}
    };

    // Normalize the kernel (make it sum to 1)
    double sum = 0;
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            sum += kernel[i][j];

    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            kernel[i][j] /= sum;

    // 2. Apply the kernel to each pixel in the image
    for (int y = 2; y < frame.rows - 2; ++y) {
        for (int x = 2; x < frame.cols - 2; ++x) {
            cv::Vec3d sum(0, 0, 0);
            for (int i = -2; i <= 2; ++i)
                for (int j = -2; j <= 2; ++j) {
                    cv::Vec3b color = frame.at<cv::Vec3b>(y + i, x + j);
                    sum += kernel[i + 2][j + 2] * cv::Vec3d(color[0], color[1], color[2]);
                }

            result.at<cv::Vec3b>(y, x) = cv::Vec3b(sum);
        }
    }

    frame = result;
}
*/
    cv::Mat result = frame.clone();

    // 1. Create a 3x3 Gaussian blur kernel
    double kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    // Normalize the kernel (make it sum to 1)
    double sum = 0;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            sum += kernel[i][j];

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            kernel[i][j] /= sum;

    // 2. Apply the kernel to each pixel in the image
    for (int y = 1; y < frame.rows - 1; ++y) {
        for (int x = 1; x < frame.cols - 1; ++x) {
            cv::Vec3d sum(0, 0, 0);
            for (int i = -1; i <= 1; ++i)
                for (int j = -1; j <= 1; ++j) {
                    cv::Vec3b color = frame.at<cv::Vec3b>(y + i, x + j);
                    sum += kernel[i + 1][j + 1] * cv::Vec3d(color[0], color[1], color[2]);
                }

            result.at<cv::Vec3b>(y, x) = cv::Vec3b(sum);
        }
    }

    frame = result;
}

#endif