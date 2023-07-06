#ifndef BACKGROUND_MASK_H
#define BACKGROUND_MASK_H

#include <opencv2/opencv.hpp>

void applyBackgroundMask(const cv::Mat& background_frame, cv::Mat& camera_frame, double threshold = 60.0) {

    cv::Mat mask(background_frame.size(), CV_8UC3, cv::Scalar(0, 0, 0));

    for (int y = 0; y < background_frame.rows; ++y) {
        for (int x = 0; x < background_frame.cols; ++x) {
            cv::Vec3b bg_pixel = background_frame.at<cv::Vec3b>(y, x);
            cv::Vec3b cam_pixel = camera_frame.at<cv::Vec3b>(y, x);

            double dist = 0.0;
            for (int i = 0; i < 3; ++i) {
                double diff = static_cast<double>(bg_pixel[i]) - static_cast<double>(cam_pixel[i]);
                dist += diff * diff;
            }

            dist = std::sqrt(dist);

            if (dist > threshold) {
                mask.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
            }
        }
    }

    camera_frame = mask.clone();
}

void applyBGBlur(cv::Mat& image, const cv::Mat& mask, int blur_size = 3) {
    cv::Mat blurred_image = image.clone();
    
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            cv::Vec3b pixel = mask.at<cv::Vec3b>(y, x);
            if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
                int sum_b = 0, sum_g = 0, sum_r = 0;
                int count = 0;

                for (int i = -blur_size; i <= blur_size; ++i) {
                    for (int j = -blur_size; j <= blur_size; ++j) {
                        int ny = y + i;
                        int nx = x + j;
                        if (0 <= ny && ny < image.rows && 0 <= nx && nx < image.cols) {
                            cv::Vec3b color = image.at<cv::Vec3b>(ny, nx);
                            sum_b += color[0];
                            sum_g += color[1];
                            sum_r += color[2];
                            count++;
                        }
                    }
                }
                
                blurred_image.at<cv::Vec3b>(y, x) = cv::Vec3b(sum_b/count, sum_g/count, sum_r/count);
            }
        }
    }

    image = blurred_image;
}

void applyBackgroundImage(cv::Mat& image, const cv::Mat& mask, const cv::Mat& background_image) {
    if (image.size() != mask.size() || image.size() != background_image.size()) {
        std::cerr << "Error: Image, mask, and background image must all be the same size." << std::endl;
        return;
    }
    cv::Mat new_image = image.clone();

    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            cv::Vec3b pixel = mask.at<cv::Vec3b>(y, x);
            if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
                new_image.at<cv::Vec3b>(y, x) = background_image.at<cv::Vec3b>(y, x);
            }
        }
    }

    image = new_image;
}


#endif


