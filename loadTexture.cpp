#include<iostream>
#include<cstring>
#include<SDL.h>
#include<SDL_ttf.h>
#include "loadTexture.h"

using namespace std;

bool LTexture::loadFromRenderedText(string textureText, SDL_Color textColor, SDL_Renderer* renderer, TTF_Font* gFont )
{
	free();
    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture != NULL )
		{
			
			//Get image dimensions
			width = textSurface->w;
			height = textSurface->h;
			// cout << textureText << " " << width << " " << height << "\n" ;
		}
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL ;
}

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	width = 0;
	height = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		width = 0;
		height = 0;
	}
}

void LTexture::render(SDL_Rect pos, SDL_Renderer* renderer) {
	int midX = pos.x + pos.w/2;
	int midY = pos.y + pos.h/2;

	SDL_Rect fillRect = {midX - width/2, midY - height/2, width, height};
	SDL_RenderCopy(renderer, mTexture, NULL, &fillRect);
}