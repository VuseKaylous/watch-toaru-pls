#ifndef MENU_H
#define MENU_H

#include "aloe.h"
#include "rushia.h"
#include "loadTexture.h"
#include "onePlayer.h"

enum listMenu {Continue, NewGame, Settings, Exit};

struct MENU {
    int listMenuSize = 4;
    LTexture listMenuTexture[4];
    SDL_Rect MenuRects[4];
    SDL_Texture *menuBackground;

    MENU();

    bool loadMenu(SDL_Renderer* renderer, TTF_Font *gFont) ;

    bool menu_event_handling(SDL_Event e, int &current_state, SDL_Renderer *renderer, ONEPLAYER &OnePlayer, SETTING &Setting) ;

    void settingUpMenu_Menu(int x, int y,int w, int h) ;

    void drawingMenu(SDL_Renderer *renderer) ;

    void MENUfree();
};



#endif