#include <SDL.h>
#include "sora.h"
#include "aloe.h"
#include "loadTexture.h"
#include "onePlayer.h"
#include "rushia.h"
#include "cursor.h"
#include "setting.h"

SETTING::SETTING() {
	
}

SETTING::~SETTING() {
    SETTINGfree();
}


int listSettingSizeX = 4, listSettingSizeY = 4;
string listSettingName[4][4] = {{"Difficulty:", "Easy", "Medium", "Hard"},{"Background:", "ver 1", "ver 2", "ver 3"}, {"Cursor:", "normal", "osu", ""}, {"Special mode:", "none", "mini mode", ""}};

void SETTING::SETTINGfree() {
    OnePlayer.ONEPLAYERfree();
    OnePlayer.mouse.CURSORfree();
    for (int i=0;i<backGifs;i++) {
        SDL_DestroyTexture( backButton[i] );
        backButton[i] = NULL;
    }
    for (int i=0;i<listSettingSizeX;i++) {
        for (int j=0;j<listSettingSizeY;j++) {
            if (listSettingName[i][j].size() == 0) continue;
            listSettingTexture[i][j].free();
        }
    }
}

bool SETTING::loadSetting(SDL_Renderer *renderer, TTF_Font *gFont) {
	bool success = true, checkSuccess = true;

    checkSuccess = OnePlayer.loadOnePlayer(renderer, gFont);
    if (!checkSuccess) success = false;
    checkSuccess = OnePlayer.mouse.loadCursor(renderer);
    if (!checkSuccess) success = false;

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
    for (int j=0;j<listSettingSizeX;j++) {
        for (int i=0;i<listSettingSizeY;i++) {
            if (listSettingName[j][i].size() > 0) {
                if (!listSettingTexture[j][i].loadFromRenderedText(listSettingName[j][i], textColor, renderer, gFont)) {
                    cout << "Failed to load texture " << listSettingName[j][i] << "\n" ;
                    success = false;
                    break;
                }
            }
        }
    }
    return success;
}

bool SETTING::setting_event_handling(SDL_Event e) {
    // OnePlayer.mouse.getExactPos(OnePlayer.miniScreen);
    if (e.type == SDL_MOUSEBUTTONUP) {
    	if (isInSDLRect(backRect, OnePlayer.mouse.realPosX, OnePlayer.mouse.realPosY)) {
            return true;
        }
        for (int i=0;i<listSettingSizeX;i++) {
            for (int j=1;j<listSettingSizeY;j++) {
                if (listSettingName[i][j].size() == 0) continue;
                if (isInSDLRect(listSettingRects[i][j], OnePlayer.mouse.realPosX, OnePlayer.mouse.realPosY) && j>0) {
                    switch (i) {
                        case 0:
                            OnePlayer.chosenDifficulty = j-1;
                            OnePlayer.restart1p();
                            break;
                        case 1:
                            OnePlayer.onePlayerChosenBackgroundX = j-1;
                            break;
                        case 2:
                            OnePlayer.mouse.usingCursor = j-1;
                            OnePlayer.mouse.setCursor();
                            break;
                        case 3:
                            chosenMouseMode = j-1;
                            OnePlayer.miniScreen.setSubScreen(j-1);
                            break;
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
    for (int i=0;i<listSettingSizeX;i++) {
        for (int j=0;j<listSettingSizeY;j++) {
            if (listSettingName[i][j].size() == 0) continue;
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

void SETTING::drawingSetting(SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, settingBackground, NULL, NULL);
    // OnePlayer.mouse.getExactPos(OnePlayer.miniScreen);
    for (int i=0;i<listSettingSizeX;i++) {
        int chosenNumber = 1;
        switch (i) {
            case 0:
                chosenNumber = OnePlayer.chosenDifficulty;
                break;
            case 1:
                chosenNumber = OnePlayer.onePlayerChosenBackgroundX;
                break;
            case 2:
                chosenNumber = OnePlayer.mouse.usingCursor;
                break;
            case 3:
                chosenNumber = chosenMouseMode;
        }
        for (int j=0;j<listSettingSizeY;j++) {
            if (listSettingName[i][j].size() == 0) continue;
            if (j>0 && isInSDLRect(listSettingRects[i][j], OnePlayer.mouse.realPosX, OnePlayer.mouse.realPosY)) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
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