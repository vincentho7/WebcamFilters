#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>
//g++ src/main.cpp -I/usr/include/SDL2 -I/usr/include/opencv4 -lSDL2 -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -o main
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

    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        cv::Mat frame;
        cap >> frame; // Capture une nouvelle image de la webcam

        //Applique un filtre à l'image ici
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);

        // Convertit l'image OpenCV en une texture SDL
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, frame.cols, frame.rows);
        SDL_UpdateTexture(texture, NULL, frame.data, frame.step);

        // Affiche l'image
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Libère la texture
        SDL_DestroyTexture(texture);

        // Attends une petite période de temps
        SDL_Delay(20);
    }

    // Nettoie et ferme
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}