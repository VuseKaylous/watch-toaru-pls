#ifndef SETTING_H
#define SETTING_H

#include "onePlayer.h"
#include "rushia.h"
#include "cursor.h"

struct SETTING {
	int backGifs = 41, chosenBackPic;
	SDL_Texture* backButton[41], *settingBackground;

	LTexture listSettingTexture[3][4];
	SDL_Rect listSettingRects[3][4];
	SDL_Rect backRect;

	SETTING();

	bool loadSetting(SDL_Renderer *renderer, TTF_Font *gFont);

	bool setting_event_handling(SDL_Event e, ONEPLAYER &OnePlayer, CURSOR &mouse) ;

	void settingUpSettings() ;

	void drawBackButton(SDL_Renderer *renderer) ;

	void drawingSetting(SDL_Renderer *renderer, ONEPLAYER &OnePlayer, CURSOR &mouse) ;

	void SETTINGfree();

	~SETTING();
};

#endif