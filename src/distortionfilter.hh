#ifndef DIST_H
#define DIST_H
#include <opencv2/opencv.hpp>
#include <random>
#include <cmath>

    void applyNoiseFilter(cv::Mat& frame, int time) {
        cv::Mat output = frame.clone();

        std::mt19937 gen(time); // Utilisez le temps comme graine pour le générateur de nombres aléatoires
        std::uniform_int_distribution<> dis(-10, 10); // Définissez l'intervalle de déplacement des pixels

        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                int newX = x + dis(gen);
                int newY = y + dis(gen);

                // Vérifiez si les nouvelles coordonnées sont à l'intérieur de la frame
                if(newX >= 0 && newX < frame.cols && newY >= 0 && newY < frame.rows)
                    output.at<cv::Vec3b>(newY, newX) = frame.at<cv::Vec3b>(y, x);
            }
        }

        frame = output;
    }


    void applyDistortionFilter(cv::Mat& frame, int time) {
        cv::Mat output = cv::Mat::zeros(frame.size(), frame.type());

        int centerX = frame.cols / 2;
        int centerY = frame.rows / 2;

        // Calculate time varying factor
        double timeFactor = (std::sin(time / 100.0) + 1) / 2; // value between 0 and 1

        // Determine zoom factor - this will need adjusting to suit your needs
        double zoomFactor = 1.5;

        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                int dx = x - centerX;
                int dy = y - centerY;

                // Calculate the distance to the center (in pixels)
                double dist = sqrt(dx*dx + dy*dy);

                // Calculate the displacement factor based on the distance
                double factor = (dist / centerX) * timeFactor;

                // Displace pixels in a non-linear manner for a mirror effect
                int newX = centerX + (dx / (1 + factor * zoomFactor));
                int newY = centerY + (dy / (1 + factor * zoomFactor));

                // Check if the new coordinates are inside the frame
                // If not, set them to the nearest edge
                newX = std::max(0, std::min(newX, frame.cols - 1));
                newY = std::max(0, std::min(newY, frame.rows - 1));

                output.at<cv::Vec3b>(newY, newX) = frame.at<cv::Vec3b>(y, x);
            }
        }

        frame = output;
    }

    void applyWaterFilter(cv::Mat& frame, int time) {
        cv::Mat output = frame.clone();

        double waveFrequency = 0.02;
        double waveAmplitude = 5;
        double waveSpeed = 0.2;

        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);

                // Adjust color to give a blue/green tint
                int b = std::min(255, static_cast<int>(color[0] * 1.2));
                int g = std::min(255, static_cast<int>(color[1] * 1.1));
                int r = std::min(255, static_cast<int>(color[2] * 0.7));

                color = cv::Vec3b(b, g, r);

                // Apply a complex sinusoidal deformation
                double dx = waveAmplitude * std::sin(2.0 * M_PI * waveFrequency * y + time * waveSpeed);
                double dy = waveAmplitude * std::cos(2.0 * M_PI * waveFrequency * x + time * waveSpeed);
                
                int newX = x + static_cast<int>(dx);
                int newY = y + static_cast<int>(dy);

                // Check if the new coordinates are inside the frame
                if(newX >= 0 && newX < frame.cols && newY >= 0 && newY < frame.rows)
                    output.at<cv::Vec3b>(newY, newX) = color;
            }
        }

        frame = output;
    }

struct Bubble {
    cv::Point center;
    cv::Point speed;
    int radius;
};

class BubbleFilter {
private:
    std::list<Bubble> bubbles;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> positionDistribution;
    std::uniform_int_distribution<int> radiusDistribution;
    std::uniform_int_distribution<int> speedDistribution;

public:
    BubbleFilter(int frameWidth) 
        : positionDistribution(0, frameWidth - 1),
          radiusDistribution(10, 50), // Pour une taille de bulle aléatoire
          speedDistribution(-2, 2) {} // Pour une vitesse de bulle aléatoire

    void applyFilter(cv::Mat& frame, cv::Mat& originalFrame, int time) {
        // Génère une nouvelle bulle toutes les secondes
        if (time % 25 == 0) {
            Bubble bubble;
            bubble.center = cv::Point(positionDistribution(generator), frame.rows - 1);
            bubble.radius = radiusDistribution(generator);
            bubble.speed = cv::Point(speedDistribution(generator), -1);
            bubbles.push_back(bubble);
        }

        // Fait monter les bulles et les dessine sur l'image finale
        for (auto it = bubbles.begin(); it != bubbles.end();) {
            Bubble& bubble = *it;

            // Fait monter la bulle avec oscillation
            bubble.center += bubble.speed;

            // Dessine la bulle avec un effet convexe
            for (int y = std::max(0, bubble.center.y - bubble.radius); y < std::min(frame.rows, bubble.center.y + bubble.radius); ++y) {
                for (int x = std::max(0, bubble.center.x - bubble.radius); x < std::min(frame.cols, bubble.center.x + bubble.radius); ++x) {
                    double distance = cv::norm(cv::Point(x, y) - bubble.center);

                    if (distance <= bubble.radius) {
                        double intensity = 1.0 - (distance / bubble.radius);
                        frame.at<cv::Vec3b>(y, x) = originalFrame.at<cv::Vec3b>(y, x) * intensity + cv::Vec3b(255, 255, 255) * (1 - intensity);
                    }
                }
            }

            // Supprime la bulle si elle est sortie de l'image
            if (bubble.center.y < -bubble.radius || bubble.center.x < -bubble.radius || bubble.center.x > frame.cols + bubble.radius) {
                it = bubbles.erase(it);
            } else {
                ++it;
            }
        }
    }
};


void applyColorFilter(cv::Mat& frame) {
    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);

            // Réduire la composante bleue
            color[0] = static_cast<uchar>(color[0] * 0.6); 

            // Augmenter la composante rouge
            color[2] = static_cast<uchar>(std::min(255.0, color[2] * 1.4));

            // Laisser la composante verte intacte
        }
    }
}

void applyHeatDistortion(cv::Mat& frame, int time) {
    cv::Mat distortedFrame = frame.clone();

    float frequency = 2 * CV_PI / frame.cols; // One full cycle per frame width
    float amplitude = 0.05f * frame.cols; // Max displacement = 5% of frame width
    float speed = 2 * CV_PI / 100; // One full cycle per 100 frames

    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            // Apply sine wave distortion
            float displacement = amplitude * std::sin(frequency * x + speed * time);

            // The displacement is larger at the bottom of the screen
            displacement *= (frame.rows - y) / static_cast<float>(frame.rows);

            // Get the source pixel position
            int srcX = x + static_cast<int>(displacement);
            srcX = std::clamp(srcX, 0, frame.cols - 1);

            // Copy pixel
            distortedFrame.at<cv::Vec3b>(y, x) = frame.at<cv::Vec3b>(y, srcX);
        }
    }

    frame = distortedFrame;
}

class FireFilter {
private:
    cv::Mat temperature;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

    cv::Vec3b interpolateColor(float temp) {
        cv::Vec3b color;
        if (temp < 85) {
            color[2] = temp * 3;
            color[1] = 0;
            color[0] = 0;
        } else if (temp < 170) {
            color[2] = 255;
            color[1] = (temp - 85) * 3;
            color[0] = 0;
        } else {
            color[2] = 255;
            color[1] = 255;
            color[0] = (temp - 170) * 3;
        }
        return color;
    }

public:
    FireFilter(int frameWidth, int frameHeight) 
        : temperature(cv::Mat::zeros(frameHeight, frameWidth, CV_32F)),
          distribution(0, 255) {}

    void applyFilter(cv::Mat& frame, int time) {
        applyColorFilter(frame);
        applyHeatDistortion(frame, time);

        // Augmenter la température en bas de l'écran
        for (int x = 0; x < frame.cols; ++x) {
            temperature.at<float>(frame.rows - 1, x) = distribution(generator);
        }

        // Propager la température vers le haut
        for (int y = 0; y < frame.rows - 1; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                float sum = 0;
                int count = 0;

                // Moyenne des températures des pixels voisins
                for (int dy = 0; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;

                        if (ny >= 0 && ny < frame.rows && nx >= 0 && nx < frame.cols) {
                            sum += temperature.at<float>(ny, nx);
                            ++count;
                        }
                    }
                }

                temperature.at<float>(y, x) = sum / count * pow(0.995f, y / static_cast<float>(frame.rows));
            }
        }

        // Draw the fire effect
        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                int heat = std::min(255, static_cast<int>(temperature.at<float>(y, x)));

                // Interpolate the color
                cv::Vec3b& color = frame.at<cv::Vec3b>(y, x);
                cv::Vec3b fireColor = interpolateColor(heat);

                // Blend the fire color with the original color
                float alpha = heat / 255.0f; // The intensity of the fire color
                color = color * (1.0f - alpha) + fireColor * alpha; // Linear interpolation (LERP)
            }
        }
    }
};

class OldFilmFilter {
private:
    std::default_random_engine rng;

public:
    OldFilmFilter() : rng(std::random_device{}()) {}

    void applyFilter(cv::Mat& frame, int time) {
        std::uniform_int_distribution<int> dist(0, 255);

        // Pour chaque pixel de l'image
        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);

                // Convertir en sépia
                int tr = std::min(255, static_cast<int>(0.393 * pixel[2] + 0.769 * pixel[1] + 0.189 * pixel[0]));
                int tg = std::min(255, static_cast<int>(0.349 * pixel[2] + 0.686 * pixel[1] + 0.168 * pixel[0]));
                int tb = std::min(255, static_cast<int>(0.272 * pixel[2] + 0.534 * pixel[1] + 0.131 * pixel[0]));
                pixel = cv::Vec3b(tb, tg, tr);
                
                // Ajouter du grain
                int noise = dist(rng) < 5 ? dist(rng) : 0;
                pixel[0] = std::clamp(pixel[0] + noise, 0, 255);
                pixel[1] = std::clamp(pixel[1] + noise, 0, 255);
                pixel[2] = std::clamp(pixel[2] + noise, 0, 255);
            }
        }

        // Vignetage
        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                double dx = static_cast<double>(x - frame.cols / 2) / (frame.cols / 2);
                double dy = static_cast<double>(y - frame.rows / 2) / (frame.rows / 2);
                double dist = sqrt(dx * dx + dy * dy)/2;

                cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);
                pixel[0] = pixel[0] * (1.0 - dist);
                pixel[1] = pixel[1] * (1.0 - dist);
                pixel[2] = pixel[2] * (1.0 - dist);
            }
        }
        if (int noise = dist(rng) < 3) {
            for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                double dx = static_cast<double>(x - frame.cols / 2) / (frame.cols / 2);
                double dy = static_cast<double>(y - frame.rows / 2) / (frame.rows / 2);
                double dist = sqrt(dx * dx + dy * dy)/2;

                cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);
                pixel[0] = pixel[0] * (1.0 + dist);
                pixel[1] = pixel[1] * (1.0 + dist);
                pixel[2] = pixel[2] * (1.0 + dist);
            }
        }
        }
        for (int x = 0; x< frame.cols; ++x) {
            if (dist(rng) < 3) {
                for (int y = 0; y < frame.rows; ++y) {
                    cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);
                    pixel[0] = 0;
                    pixel[1] = 0;
                    pixel[2] = 0;
                }
            }
        }
        
    }
    
};

class PolygonizeFilter {
public:
    void applyFilter(cv::Mat& frame) {
        int blockSize = 10; // Taille des blocs de pixels à regrouper ensemble

        for (int y = 0; y < frame.rows; y += blockSize) {
            for (int x = 0; x < frame.cols; x += blockSize) {
                cv::Vec3f averageColor = calculateAverageColor(frame, x, y, blockSize);
                fillBlock(frame, x, y, blockSize, averageColor);
            }
        }
    }

    cv::Vec3f calculateAverageColor(const cv::Mat& frame, int startX, int startY, int blockSize) {
        cv::Vec3f sumColor(0, 0, 0);
        int count = 0;

        for (int y = startY; y < startY + blockSize && y < frame.rows; ++y) {
            for (int x = startX; x < startX + blockSize && x < frame.cols; ++x) {
                sumColor += frame.at<cv::Vec3b>(y, x);
                ++count;
            }
        }

        return sumColor / count;
    }

    void fillBlock(cv::Mat& frame, int startX, int startY, int blockSize, const cv::Vec3f& color) {
        for (int y = startY; y < startY + blockSize && y < frame.rows; ++y) {
            for (int x = startX; x < startX + blockSize && x < frame.cols; ++x) {
                frame.at<cv::Vec3b>(y, x) = color;
            }
        }
    }
};
    



#endif