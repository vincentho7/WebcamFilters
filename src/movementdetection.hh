#ifndef MOVEMENT_DETECTION_H
#define MOVEMENT_DETECTION_H

#include <opencv2/opencv.hpp>
#include <cmath>
class MovementDetection {
private:
    cv::Mat prevFrame;

public:
    // Constructeur par défaut
    MovementDetection() {}

    // Méthode pour appliquer le filtre de détection de mouvement
    void applyFilter(cv::Mat& frame) {
        if (prevFrame.empty()) {
            // Si c'est la première frame, il n'y a rien à comparer
            // Donc nous la stockons pour la prochaine itération
            prevFrame = frame.clone();
            return;
        }
        cv::Mat save = frame.clone();
        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
                cv::Vec3b& prevColor = prevFrame.at<cv::Vec3b>(y, x);

                // Calculer la différence absolue pour chaque canal de couleur
                int diffB = abs(color[0] - prevColor[0]);
                int diffG = abs(color[1] - prevColor[1]);
                int diffR = abs(color[2] - prevColor[2]);

                color[0] = (diffB > 30) ? 255 : 0;
                color[1] = (diffG > 30) ? 255 : 0;
                color[2] = (diffR > 30) ? 255 : 0;
            }
        }

        // Copie la frame actuelle pour l'utiliser lors de la prochaine itération
        prevFrame = save.clone();
    }

    void applyFusionFilter(cv::Mat& frame) {
    if (prevFrame.empty()) {
        // Si c'est la première frame, il n'y a rien à fusionner
        // Donc nous la stockons pour la prochaine itération
        prevFrame = frame.clone();
        return;
    }

    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
            cv::Vec3b& prevColor = prevFrame.at<cv::Vec3b>(y, x);

            // Fusionner les deux frames en faisant la moyenne de leurs valeurs de couleur
            color[0] = (color[0] + prevColor[0]) / 2;
            color[1] = (color[1] + prevColor[1])/ 2;
            color[2] = (color[2] + prevColor[2]) / 2;
        }
    }

    // Copie la frame actuelle pour l'utiliser lors de la prochaine itération
    prevFrame = frame.clone();
    }

    void applyNoRefreshFilter(cv::Mat& frame) {
    if (prevFrame.empty()) {
        // Si c'est la première frame, il n'y a rien à fusionner
        // Donc nous la stockons pour la prochaine itération
        prevFrame = frame.clone();
        return;
    }

    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
            cv::Vec3b& prevColor = prevFrame.at<cv::Vec3b>(y, x);

            // Fusionner les deux frames en faisant la moyenne de leurs valeurs de couleur
            color[0] = (color[0] *0.1 + prevColor[0] *0.9);
            color[1] = (color[1] *0.1 + prevColor[1]* 0.9);
            color[2] = (color[2] * 0.1 + prevColor[2] * 0.9);
        }
    }

    // Copie la frame actuelle pour l'utiliser lors de la prochaine itération
    prevFrame = frame.clone();
    }
};

#endif

