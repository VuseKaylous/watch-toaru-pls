#ifndef SUBSCREEN_H
#define SUBSCREEN_H

#include<SDL.h>
#include<iostream>

struct SUBSCREEN {
	SDL_Rect boingboing;
	SDL_Rect mainScreen;
	SUBSCREEN();
	SUBSCREEN(int width, int height);
	void settingUpSubScreen(int width, int height);
	void setSubScreen(int mode);
	void subScreen_event_handling(int x, int y);
};

#endif