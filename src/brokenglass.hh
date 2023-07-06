#include <random>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
class BrokenGlassFilter {
private:
    // Structure pour contenir les informations de chaque "éclat" de verre
    struct Shard {
        int srcX, srcY; // Origine de l'éclat dans l'image source
        int destX, destY; // Destination de l'éclat dans l'image de sortie
        int width, height; // Largeur et hauteur de l'éclat
    };

    std::vector<Shard> shards; // Vecteur pour stocker les éclats de verre

public:
    // Constructeur qui crée les éclats de verre
    BrokenGlassFilter(int numShards, int frameWidth, int frameHeight) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, std::min(frameWidth, frameHeight));

        for (int i = 0; i < numShards; ++i) {
            Shard shard;
            shard.srcX = dis(gen);
            shard.srcY = dis(gen);
            shard.destX = dis(gen);
            shard.destY = dis(gen);
            shard.width = dis(gen);
            shard.height = dis(gen);
            shards.push_back(shard);
        }
    }

    // Applique le filtre à une frame
    void applyFilter(cv::Mat& frame) {
        cv::Mat output = frame.clone();

        for (const Shard& shard : shards) {
            for (int y = 0; y < shard.height; ++y) {
                for (int x = 0; x < shard.width; ++x) {
                    if (shard.srcY + y < frame.rows && shard.srcX + x < frame.cols
                        && shard.destY + y < frame.rows && shard.destX + x < frame.cols) {
                        output.at<cv::Vec3b>(shard.destY + y, shard.destX + x) =
                            frame.at<cv::Vec3b>(shard.srcY + y, shard.srcX + x);
                    }
                }
            }
        }

        frame = output.clone();
    }
};