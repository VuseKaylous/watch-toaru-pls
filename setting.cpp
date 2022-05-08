#include <SDL.h>
#include "sora.h"
#include "aloe.h"
#include "loadTexture.h"
#include "onePlayer.h"
#include "rushia.h"
#include "setting.h"

SETTING::SETTING() {
	
}

void SETTING::SETTINGfree() {
    for (int i=0;i<backGifs;i++) {
        SDL_DestroyTexture( backButton[i] );
        backButton[i] = NULL;
    }
    for (int i=0;i<2;i++) {
        for (int j=0;j<4;j++) {
            listSettingTexture[i][j].free();
        }
    }
}

string listSettingName[2][4] = {{"Difficulty:", "Easy", "Medium", "Hard"},{"Background:", "ver 1", "ver 2", "ver 3"}};

bool SETTING::loadSetting(SDL_Renderer *renderer, TTF_Font *gFont) {
	bool success = true;
	string loadingPictures;
	loadingPictures = "picture/SDL_image_related/back/";
    for (int i=0;i<backGifs;i++) {
        string si;
        si = loadingPictures + toString(i) + ".png" ;
        backButton[i] = loadSurface(si,renderer);
        if (backButton[i] == NULL) {
            cout << "Failed to load image back number " << i << "\n" ;
            success = false;
            break;
        }
    }
    SDL_Color textColor = { 255, 255, 255 };
    for (int j=0;j<2;j++) {
        for (int i=0;i<4;i++) {
            if (!listSettingTexture[j][i].loadFromRenderedText(listSettingName[j][i], textColor, renderer, gFont)) {
                cout << "Failed to load texture " << listSettingName[j][i] << "\n" ;
                success = false;
                break;
            }
        }
    }
    return success;
}

bool SETTING::setting_event_handling(SDL_Event e, ONEPLAYER &OnePlayer, BOARD &board) {
    if (e.type == SDL_MOUSEBUTTONUP) {
    	if (isInSDLRect(backRect)) {
            return true;
        }
        for (int i=0;i<2;i++) {
            for (int j=1;j<4;j++) {
                if (isInSDLRect(listSettingRects[i][j]) && j>0) {
                    if (i==0) {
                        OnePlayer.chosenDifficulty = j-1;
                        OnePlayer.restart1p(board);
                    } else {
                        OnePlayer.onePlayerChosenBackgroundX = j-1;
                    }
                }
            }
        }
    }
    return false;
}

void SETTING::settingUpSettings() {
    int x,y;
    y = listSettingTexture[0][0].height*2;
    x = y;
    for (int i=0;i<2;i++) {
        for (int j=0;j<4;j++) {
            listSettingRects[i][j] = {x, y, listSettingTexture[i][j].width + listSettingTexture[i][j].height, listSettingTexture[i][j].height*2};
            x += listSettingRects[i][j].w + 30;
        }
        y += listSettingRects[i][0].h + 30 ;
        x = listSettingTexture[0][0].height*2;
    }
}

void SETTING::drawBackButton(SDL_Renderer *renderer) {
    chosenBackPic = (chosenBackPic+1)%(backGifs*2);
    SDL_RenderCopy(renderer, backButton[chosenBackPic/2], NULL, &backRect);
}

void SETTING::drawingSetting(SDL_Renderer *renderer, ONEPLAYER OnePlayer) {
    SDL_RenderCopy(renderer, settingBackground, NULL, NULL);
    
    for (int i=0;i<2;i++) {
        int chosenNumber = 1;
        if (i==0) chosenNumber = OnePlayer.chosenDifficulty;
        if (i==1) chosenNumber = OnePlayer.onePlayerChosenBackgroundX;
        for (int j=0;j<4;j++) {
            if (j>0 && isInSDLRect(listSettingRects[i][j])) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else {
                if (j-1 == chosenNumber) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            SDL_RenderDrawRect(renderer, &listSettingRects[i][j]);
            listSettingTexture[i][j].render(listSettingRects[i][j], renderer);
            // cout << "rendered background setting\n" ;
        }
    }
    drawBackButton(renderer);
}