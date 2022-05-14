#ifndef CURSOR_H
#define CURSOR_H

#include<SDL.h>
#include "subScreen.h"

struct CURSOR {
	int tracesSize, changingTrace;
	SDL_Point traces[20];
	SDL_Texture *cursorTrail;

	int usingCursor;
	SDL_Cursor *osuCursor, *normalCursor;

	int posX, posY, realPosX, realPosY;

	CURSOR();
	bool loadCursor(SDL_Renderer *renderer);

	void CURSORfree();
	void setCursor();
	void getExactPos(SUBSCREEN smolRect);
	void cursor_event_handling();
	void drawCursor(SDL_Renderer *renderer);
	void drawPoint(SDL_Renderer *renderer, SDL_Point p);
};

#endif