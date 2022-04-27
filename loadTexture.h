#ifndef LOADTEXTURE_H
#define LOADTEXTURE_H

#include<iostream>
#include<cstring>
#include<SDL.h>
#include<SDL_ttf.h>

using namespace std;

struct LTexture {
	int width,height;
	SDL_Texture* mTexture;
	LTexture();
	~LTexture();
	void free();
	bool loadFromRenderedText(string textureText, SDL_Color textColor, SDL_Renderer* renderer, TTF_Font* gFont );
	void render(SDL_Rect pos, SDL_Renderer* renderer);
};

#endif