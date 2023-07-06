#ifndef SOBEL_FILTER_H
#define SOBEL_FILTER_H

#include <opencv2/opencv.hpp>
#include "grayscale.hh"
void applySobelEdgeDetection(cv::Mat& frame) {
    cv::Mat frame_gray = frame.clone();

    // Convert the image to grayscale
    applyGrayscale(frame_gray);

    // Sobel Operator Kernel
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    cv::Mat result = frame_gray.clone();

    for (int y = 1; y < frame_gray.rows - 1; y++) {
        for (int x = 1; x < frame_gray.cols - 1; x++) {
            int dx = (
                Gx[0][0] * frame_gray.at<cv::Vec3b>(y-1, x-1)[0] +
                Gx[0][1] * frame_gray.at<cv::Vec3b>(y-1, x)[0] +
                Gx[0][2] * frame_gray.at<cv::Vec3b>(y-1, x+1)[0] +
                Gx[1][0] * frame_gray.at<cv::Vec3b>(y, x-1)[0] +
                Gx[1][1] * frame_gray.at<cv::Vec3b>(y, x)[0] +
                Gx[1][2] * frame_gray.at<cv::Vec3b>(y, x+1)[0] +
                Gx[2][0] * frame_gray.at<cv::Vec3b>(y+1, x-1)[0] +
                Gx[2][1] * frame_gray.at<cv::Vec3b>(y+1, x)[0] +
                Gx[2][2] * frame_gray.at<cv::Vec3b>(y+1, x+1)[0]
            );

            int dy = (
                Gy[0][0] * frame_gray.at<cv::Vec3b>(y-1, x-1)[0] +
                Gy[0][1] * frame_gray.at<cv::Vec3b>(y-1, x)[0] +
                Gy[0][2] * frame_gray.at<cv::Vec3b>(y-1, x+1)[0] +
                Gy[1][0] * frame_gray.at<cv::Vec3b>(y, x-1)[0] +
                Gy[1][1] * frame_gray.at<cv::Vec3b>(y, x)[0] +
                Gy[1][2] * frame_gray.at<cv::Vec3b>(y, x+1)[0] +
                Gy[2][0] * frame_gray.at<cv::Vec3b>(y+1, x-1)[0] +
                Gy[2][1] * frame_gray.at<cv::Vec3b>(y+1, x)[0] +
                Gy[2][2] * frame_gray.at<cv::Vec3b>(y+1, x+1)[0]
            );

            int val = sqrt(dx * dx + dy * dy);

            // Truncate values above 255
            result.at<cv::Vec3b>(y, x) = val > 255 ? cv::Vec3b(255, 255, 255) : cv::Vec3b(val, val, val);
        }
    }

    frame = result;
}


#endif
