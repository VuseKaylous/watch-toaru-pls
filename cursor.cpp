#include <cstdio>
#include <SDL_image.h>
#include "sora.h"
#include "cursor.h"

CURSOR::CURSOR(){
	usingCursor = 0;
	tracesSize = 20;
	int x,y;
    SDL_GetMouseState(&x,&y);
    for (int i=0;i<tracesSize;i++) traces[i] = {x,y};
}

bool CURSOR::loadCursor(SDL_Renderer *renderer) {
	bool success = true;
	osuCursor = SDL_CreateColorCursor(IMG_Load( "picture/cursor/cursormiddle.png" ), 27, 27);
	if (osuCursor == NULL) success = false;
	normalCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	// SDL_SetCursor(osuCursor);
	setCursor();
	cursorTrail = loadSurface("picture/cursor/cursortrail.png", renderer);
	if (cursorTrail == NULL) success = false;
	return success;
}

void CURSOR::setCursor() {
	if (usingCursor == 0) {
		if (normalCursor != SDL_GetCursor()) SDL_SetCursor(normalCursor);
	}
	if (usingCursor == 1) {
		if (osuCursor != SDL_GetCursor()) SDL_SetCursor(osuCursor);
	}
}

void CURSOR::CURSORfree() {
	SDL_FreeCursor(osuCursor);
	SDL_FreeCursor(normalCursor);
	SDL_DestroyTexture(cursorTrail);
	cursorTrail = NULL;
}

void CURSOR::cursor_event_handling() {
	int x,y;
	SDL_GetMouseState(&x, &y);
	for (int i=19;i>0;i--) traces[i] = traces[i-1];
	traces[0] = {x,y};
}
void CURSOR::drawPoint(SDL_Renderer *renderer, SDL_Point p) {
	SDL_Rect fillRect = {p.x, p.y, 8, 8};
	SDL_RenderCopy(renderer, cursorTrail, NULL, &fillRect);
	/*
	int xi[] = {0,1,0,-1,0}, yi[] = {0,0,1,0,-1};
	SDL_RenderDrawPoint(renderer, p.x, p.y);
	for (int i=0;i<5;i++) {
		SDL_RenderDrawPoint(renderer, p.x + xi[i], p.y + yi[i]);

	}
	*/
}
void CURSOR::drawCursor(SDL_Renderer *renderer) {
	int rt = 0;
	SDL_SetRenderDrawColor(renderer, 255,255,255, 255);
	for (int i=0;i<tracesSize;i++) drawPoint(renderer, traces[i]);

	// int xi[] = {0,1,2,1,0,-1,-2,-1}, yi[] = {2,1,0,-1,-2,-1,0,1};
	// for (int i=0;i<8;i++) SDL_RenderDrawPoint(renderer, traces[i].x + xi[i], traces[i].y + yi[i]);

	// rt = SDL_RenderDrawPoints(renderer, traces, tracesSize);
	// if (rt<0) {
	// 	printf("failure in draw points\n");
	// }
}