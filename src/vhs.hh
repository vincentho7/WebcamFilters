#ifndef VHS_H
#define VHS_H
#include <opencv2/opencv.hpp>
#include <random>
#include <cmath>

#include <deque>

class VHSFilter {
public:
    VHSFilter() : distribution(-10, 10), distortion_counter(0), distortion_start(-1), distortion_end(-1), distortion_type(0) {}

    void apply(cv::Mat& frame, double time) {
        cv::Mat original = frame.clone();  // Faire une copie de l'image originale

        for (int y = 0; y < frame.rows; ++y) {
       
            // Décider aléatoirement si la ligne doit être blanche
            bool isWhiteLine = std::rand() % 3000 == 0;

            for (int x = 0; x < frame.cols; ++x) {
            
                cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
                if (isWhiteLine) {
                    color = cv::Vec3b(255, 255, 255);
                    continue;
                }

                // Ajout de la distorsion
                if (distortion_counter > 0 && y >= int(distortion_start + time*(10))% frame.rows && y <= int(distortion_end + time*(10))% frame.rows) {
                    if(distortion_type == 0)
                        applyDistortionType1(frame, original, x, y);
                    else if(distortion_type == 1)
                        applyDistortionType2(frame, original, x, y);
                    else if(distortion_type == 2)
                        applyColorShift(frame, original, x, y);
                }

                // Simulation de la fluctuation des couleurs
                int fluctuation = std::sin(time) * 10;
                color[0] = std::clamp(color[0] + fluctuation, 0, 255);
                color[1] = std::clamp(color[1] + fluctuation, 0, 255);
                color[2] = std::clamp(color[2] + fluctuation, 0, 255);

                // Simulation de la fluctuation des couleurs
                int fluctuationR = distribution(generator) % 10;
                int fluctuationG = distribution(generator) % 10;
                int fluctuationB = distribution(generator) % 10;
                color[0] = std::clamp(color[0] + fluctuationR, 0, 255);
                color[1] = std::clamp(color[1] + fluctuationG, 0, 255);
                color[2] = std::clamp(color[2] + fluctuationB, 0, 255);
            }
        }

        // Gestion de la durée de la distorsion
        if (distortion_counter > 0) {
            --distortion_counter;
        } else {
            // Décider aléatoirement quand commencer la prochaine distorsion
            if (distribution(generator) > 4) {
                distortion_counter = (distribution(generator)*30 % 50) + 10;
                distortion_start = distribution(generator)*30 % frame.rows;
                distortion_end = distortion_start + (distribution(generator)*5 % 50) + 10;
                distortion_type = distribution(generator) % 3; // Choisir aléatoirement le type de distorsion
            }
        }
    }

private:
    void applyDistortionType1(cv::Mat& frame, cv::Mat& original, int x, int y) {
        // Insérez le code pour le type de distorsion 1 ici
        int original_x = x + std::sin( y / 50.0) * 20;
        if (original_x < 0 || original_x >= frame.cols) return;
        frame.at<cv::Vec3b>(y, x) = original.at<cv::Vec3b>(y, original_x);
    }

    void applyDistortionType2(cv::Mat& frame, cv::Mat& original, int x, int y) {
        // Insérez le code pour le type de distorsion 2 ici
        // Exemple : distorsion avec une fonction sinusoïdale différente
        int original_x = x + std::sin( y / 30.0) * 30;
        if (original_x < 0 || original_x >= frame.cols) return;
        frame.at<cv::Vec3b>(y, x) = original.at<cv::Vec3b>(y, original_x);
    }

    void applyColorShift(cv::Mat& frame, cv::Mat& original, int x, int y) {
        // Distorsion par décalage des canaux de couleur
        cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);

        // Calculer les nouvelles coordonnées pour chaque canal de couleur
        int original_x_r = std::clamp(x + distribution(generator) % 5, 0, frame.cols-1);
        int original_x_g = std::clamp(x + distribution(generator) % 5, 0, frame.cols-1);
        int original_x_b = std::clamp(x + distribution(generator) % 5, 0, frame.cols-1);

        // Assigner les valeurs de couleur de l'image originale avec décalage
        color[0] = original.at<cv::Vec3b>(y, original_x_b)[0];
        color[1] = original.at<cv::Vec3b>(y, original_x_g)[1];
        color[2] = original.at<cv::Vec3b>(y, original_x_r)[2];
    }

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

    int distortion_counter;
    int distortion_start;
    int distortion_end;
    int distortion_type;
};
#endif