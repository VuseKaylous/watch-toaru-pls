#ifndef CURSOR_H
#define CURSOR_H

#include<SDL.h>

struct CURSOR {
	int tracesSize;
	SDL_Point traces[20];
	SDL_Texture *cursorTrail;

	int usingCursor;
	SDL_Cursor *osuCursor, *normalCursor;

	CURSOR();
	bool loadCursor(SDL_Renderer *renderer);

	void CURSORfree();
	void setCursor();
	void cursor_event_handling();
	void drawCursor(SDL_Renderer *renderer);
	void drawPoint(SDL_Renderer *renderer, SDL_Point p);
};

#endif