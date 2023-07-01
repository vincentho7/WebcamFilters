#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>
#include "grayscale.hh"
#include "negative.hh"
#include "bgblur.hh"
//g++ src/main.cpp -I/usr/include/SDL2 -I/usr/include/opencv4 -lSDL2 -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -o main


enum Filter {
    NONE,
    GRAYSCALE,
    INVERT,
    BGBLUR
};

Filter currentFilter = Filter::NONE;


int main(int argc, char** argv) {
    
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
    cap >> frame; // Capture une nouvelle image de la webcam

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
                case SDLK_4: currentFilter = Filter::BGBLUR; break;
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
        case Filter::BGBLUR:
            applyOtsuThresholdAndBlur(frame);
            break;
    }

    // Met à jour la texture avec les données de la nouvelle image
    SDL_UpdateTexture(texture, NULL, frame.data, frame.step);

    // Affiche l'image
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Attends une petite période de temps
    SDL_Delay(33);
}

    // Libère la texture
    SDL_DestroyTexture(texture);

    // Nettoie et ferme
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}