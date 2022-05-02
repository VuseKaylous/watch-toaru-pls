#ifndef SORA_H
#define SORA_H
#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>

using namespace std;

void logSDLError(std::ostream& os, const std::string &msg, bool fatal);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer, int SCREEN_WIDTH, int SCREEN_HEIGHT,string WINDOW_TITLE) ;

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);


SDL_Texture* loadSurface( std::string path , SDL_Renderer* renderer);

#endif