#include <SDL.h>
#include "aloe.h"
#include "rushia.h"
#include "loadTexture.h"
#include "onePlayer.h"
#include "setting.h"
#include "menu.h"

string listMenuName[] = {"Continue", "New game", "Settings", "Exit"} ;

MENU::MENU() {

}

void MENU::MENUfree() {
    for (int i=0;i<listMenuSize;i++) {
        listMenuTexture[i].free();
    }
}

bool MENU::loadMenu(SDL_Renderer* renderer, TTF_Font *gFont) {
    bool success = true;
    SDL_Color textColor = { 255, 255, 255 };
    for (int i=0;i<listMenuSize;i++) {
        // listMenuTexture[i] = loadFromRenderedText(listMenuName[i], textColor, renderer, gFont);
        if (!listMenuTexture[i].loadFromRenderedText(listMenuName[i], textColor, renderer, gFont)) {
            cout << "Failed to load texture " << listMenuName[i] << "\n" ;
            success = false;
            break;
        }
    }
    return success;
}

bool MENU::menu_event_handling(SDL_Event e, int &current_state, SDL_Renderer *renderer) {
    bool rt = false;
    // Setting.OnePlayer.mouse.getExactPos(Setting.OnePlayer.miniScreen);
    if (e.type == SDL_MOUSEBUTTONUP) {
        // int x,y;
        // SDL_GetMouseState(&x,&y);
        for (int i=0;i<listMenuSize;i++) {
            // SDL_Rect fillRect = {SCREEN_WIDTH/2 - maxWidth/2,topLeftY + i*(20 + maxHeight), maxWidth, maxHeight};
            if (isInSDLRect(MenuRects[i], Setting.OnePlayer.mouse.realPosX, Setting.OnePlayer.mouse.realPosY)) {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                // SDL_RenderClear(renderer);
                switch (i) {
                    case Continue:
                        current_state = 1 ; // onePlayerScreen
                        break;
                    case NewGame :
                        current_state = 1 ; // onePlayerScreen
                        Setting.OnePlayer.restart1p() ;
                        break;
                    case Settings :
                        current_state = 2 ; // settingScreen
                        Setting.chosenBackPic = 0;
                        break;
                    case Exit :
                        rt = true;
                        break;
                }
                break;
            }
        }
    }
    return rt;
}

void MENU::settingUpMenu_Menu(int x, int y,int w, int h) {
    int maxWidth = 0, maxHeight;
    for (int i=0;i<listMenuSize;i++) {
        maxWidth = max(maxWidth, listMenuTexture[i].width);
    }
    maxHeight = listMenuTexture[0].height*2;
    maxWidth += maxHeight;
    int topLeftX, topLeftY;
    if (listMenuSize%2==0) {
        topLeftY = h/2-y - (maxHeight + 20)*(listMenuSize/2)+10;
    } else {
        topLeftY = h/2-y - (maxHeight + 20)*(listMenuSize/2) - maxHeight/2;
    }
    for (int i=0;i<listMenuSize;i++) {
        MenuRects[i] = {w/2 - maxWidth/2,topLeftY + i*(20 + maxHeight), maxWidth, maxHeight};
    }
}

void MENU::drawingMenu(SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
    for (int i=0;i<listMenuSize;i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &MenuRects[i]);
        listMenuTexture[i].render(MenuRects[i], renderer);
    }
}