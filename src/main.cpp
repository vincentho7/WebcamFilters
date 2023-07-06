#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>
#include "grayscale.hh"
#include "negative.hh"
#include "bgblur.hh"
#include "vhs.hh"
#include "sobel.hh"
#include "fisheye.hh"
#include "mask.hh"
#include "movementdetection.hh"
#include "brokenglass.hh"
#include "distortionfilter.hh"
#include "stdio.h"
//g++ src/main.cpp -I/usr/include/SDL2 -I/usr/include/opencv4 -lSDL2 -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -o main


enum Filter {
    NONE,
    GRAYSCALE,
    INVERT,
    BLUR,
    VHS,
    SOBEL,
    FISHEYE,
    MASK,
    BGBLUR,
    BGIMAGE,
    MD,
    FUSION,
    NOREFRESH,
    BROKENGLASS,
    DISTORTION,
    NOISE,
    WATER,
    FIRE
    //cinema
    //histo
    
    //polygoniser
};

Filter currentFilter = Filter::NONE;


int main(int argc, char** argv) {

    std::string image_path = "./fond/fond.jpg";
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    VHSFilter vhs = VHSFilter();
    MovementDetection MD = MovementDetection();
    BrokenGlassFilter glass = BrokenGlassFilter(140, 640, 480);
    BubbleFilter bubbleFilter(640);
    FireFilter firefilter(640, 480);

    double time = 0.0;
    
    // Initialise OpenCV et SDL
    cv::VideoCapture cap(0, cv::CAP_V4L); // Ouvre la webcam par défaut
    
    SDL_Init(SDL_INIT_VIDEO);

    // Crée une fenêtre SDL
    SDL_Window* window = SDL_CreateWindow("Webcam", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if(!cap.isOpened()) { // Vérifie si nous avons réussi à ouvrir la webcam
        return -1;
    }

    cv::Mat frame;
    cv::Mat oframe;
    cap >> frame; // Capture une nouvelle image de la webcam
    // Obtient la résolution de la frame
    cv::Size frameSize = frame.size();
    int frameWidth = frameSize.width;
    int frameHeight = frameSize.height;
    std::cout << "Résolution de la frame : " << frameWidth << " x " << frameHeight << std::endl;

    // Crée une texture SDL une seule fois au début
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, frame.cols, frame.rows);

    bool running = true;
    while(running) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running = false;
        }
        // handle keypresses to change filter
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_1: currentFilter = Filter::NONE; break;
                case SDLK_2: currentFilter = Filter::GRAYSCALE; break;
                case SDLK_3: currentFilter = Filter::INVERT; break;
                case SDLK_4: currentFilter = Filter::BLUR; break;
                case SDLK_5: currentFilter = Filter::VHS; break;
                case SDLK_6: currentFilter = Filter::SOBEL; break;
                case SDLK_7: currentFilter = Filter::FISHEYE; break;
                case SDLK_8: currentFilter = Filter::MASK; oframe = frame.clone(); break;
                case SDLK_9: currentFilter = Filter::BGBLUR; oframe = frame.clone(); break;
                case SDLK_0: currentFilter = Filter::BGIMAGE; oframe = frame.clone(); break;
                case SDLK_a: currentFilter = Filter::MD; break;
                case SDLK_b: currentFilter = Filter::FUSION; break;
                case SDLK_c: currentFilter = Filter::NOREFRESH; break;
                case SDLK_d: currentFilter = Filter::BROKENGLASS; break;
                case SDLK_e: currentFilter = Filter::DISTORTION; break;
                case SDLK_f: currentFilter = Filter::NOISE; break;
                case SDLK_g: currentFilter = Filter::WATER; break;
                case SDLK_h: currentFilter = Filter::FIRE; break;
            }
        }
    }

    cap >> frame; // Capture une nouvelle image de la webcam

    // Applique un filtre à l'image ici
    switch(currentFilter) {
        case Filter::GRAYSCALE:
            applyGrayscale(frame);
            break;
        case Filter::INVERT:
            applyNegative(frame);
            break;
        case Filter::BLUR:
            applyBlur(frame);
            break;
        case Filter::VHS:
            vhs.apply(frame,time);
            break;
        case Filter::SOBEL:
            applySobelEdgeDetection(frame);
            break;
        case Filter::FISHEYE:
            applyFisheye(frame);
            break;
        case Filter::MASK:
            applyBackgroundMask(oframe, frame);
            break;
        case Filter::BGBLUR: {
            cv::Mat mask = frame.clone();
            applyBackgroundMask(oframe, mask);
            applyBGBlur(frame,mask,3);
            break;
        }
        case Filter::BGIMAGE:  {
            cv::Mat mask2 = frame.clone();
            applyBackgroundMask(oframe, mask2);
            applyBackgroundImage(frame,mask2,image);
            break;
        }
        case Filter::MD: {
            MD.applyFilter(frame);
            break;
        }
        case Filter::FUSION: {
            MD.applyFusionFilter(frame);
            break;
        }
        case Filter::NOREFRESH: {
            MD.applyNoRefreshFilter(frame);
            break;
        }
        case Filter::BROKENGLASS: {
            glass.applyFilter(frame);
            break;
        }
        case Filter::DISTORTION: {
            applyDistortionFilter(frame,time);
            break;
        }
        case Filter::NOISE: {
            applyNoiseFilter(frame,time);
            break;
        }
        case Filter::WATER: {
            oframe = frame.clone();
            applyWaterFilter(frame,time);
            bubbleFilter.applyFilter(frame,oframe,time);
            break;
        }
        case Filter::FIRE: {
            firefilter.applyFilter(frame,time);
            break;
        }
    }

    // Met à jour la texture avec les données de la nouvelle image
    SDL_UpdateTexture(texture, NULL, frame.data, frame.step);

    // Affiche l'image
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Attends une petite période de temps
    SDL_Delay(20);
    time += 1;
}

    // Libère la texture
    SDL_DestroyTexture(texture);

    // Nettoie et ferme
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}