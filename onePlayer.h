#ifndef ONEPLAYER_H
#define ONEPLAYER_H

#include <SDL.h>
#include "loadTexture.h"
#include "rushia.h"

// using namespace std;

/*
enum png {RestartButton, winning, menu, menuBackground} ;
string picChar[] = {"RestartButton", "winning", "menu", "menuBackground.jpg"};
int picCharSize = 4;
SDL_Texture* pic[4];
*/

struct ONEPLAYER {
	const int onePlayerBackgroundSize = 4;
	
	SDL_Texture* onePlayerBackgroundTexture[2][4];
	SDL_Texture* onePlayerBackgroundTextureGif[39];
	SDL_Texture* onePlayerDecoratingTextureGif[3];
	SDL_Texture *RestartButton, *winning;
	LTexture menu;

	SDL_Rect RestartRect, playField, MenuRect, walfieRect, walfieMovingRect;
	int onePlayerChosenBackgroundX, onePlayerChosenBackgroundY, chosenWalfie;
	float winningShowUp, winningOpacity;
	int chosenDifficulty;

	ONEPLAYER();
	void ONEPLAYERfree();
	bool loadOnePlayer(SDL_Renderer *renderer, TTF_Font *gFont);
	void restart1p(BOARD &board);
	void board_event_handling(BOARD &board, SDL_Event e);
	void drawWalfie(SDL_Renderer *renderer);
	void drawOnePlayer(BOARD &board, SDL_Renderer *renderer, bool MouseDown);
};

#endif