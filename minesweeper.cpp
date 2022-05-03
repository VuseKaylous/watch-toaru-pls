#include<iostream>
#include<vector>
#include<ctime>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "sora.h"
#include "aloe.h"
#include "rushia.h"
#include "loadTexture.h"
#include "onePlayer.h"

using namespace std;

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 640;
const string WINDOW_TITLE = "Haachamachama!!!";

SDL_Window* window;
SDL_Renderer* renderer;
// SDL_Surface* screen ;
SDL_Event e;
TTF_Font *gFont = NULL;

BOARD board;
// ONEPLAYER OnePlayer;

SDL_Texture *menuBackground;

int backGifs = 41, chosenBackPic;
SDL_Texture* backButton[41];

enum screenState {menuScreen, onePlayerScreen, settingScreen};
int current_state = menuScreen;

bool MouseIsDown = false;


enum listMenu {Continue, NewGame, Settings, Exit};
string listMenuName[] = {"Continue", "New game", "Settings", "Exit"} ;
int listMenuSize = 4;
LTexture listMenuTexture[4];
SDL_Rect MenuRects[4];

string listSettingName[2][4] = {{"Difficulty:", "Easy", "Medium", "Hard"},{"Background:", "ver 1", "ver 2", "ver 3"}} ;
LTexture listSettingTexture[2][4];
SDL_Rect listSettingRects[2][4];
const SDL_Rect backRect = {SCREEN_WIDTH - 200, SCREEN_HEIGHT - 200, 200, 200};


bool quit = false;

// time_t startTime,endTime;


//----------------------------------------- SDL preparations -------------------------------------------

bool loadMedia()
{
    bool success = true;
    bool checkSuccess = board.loadRushia(renderer);
    if (!checkSuccess) success = false;
    // cout << "fuck\n" ;
    // checkSuccess = OnePlayer.loadOnePlayer(renderer);
    // if (!checkSuccess) success = false;
    string loadingPictures;

    loadingPictures = "picture/SDL_image_related/menuBackground.jpg";
    menuBackground = loadSurface(loadingPictures, renderer);
    if (menuBackground == NULL) success = false;
    
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

    gFont = TTF_OpenFont( "fonts/lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 255, 255, 255 };
        for (int i=0;i<listMenuSize;i++) {
            // listMenuTexture[i] = loadFromRenderedText(listMenuName[i], textColor, renderer, gFont);
            if (!listMenuTexture[i].loadFromRenderedText(listMenuName[i], textColor, renderer, gFont)) {
                cout << "Failed to load texture " << listMenuName[i] << "\n" ;
                success = false;
                break;
            }
        }
        for (int j=0;j<2;j++) {
            for (int i=0;i<4;i++) {
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

//------------------------------------------------------------ real coding part ----------------------------------------------------------

void settingUpOnePerson() {
    board.squareSize = SCREEN_WIDTH/board.Cols;
    // OnePlayer.playField = {0,SCREEN_HEIGHT - board.Rows * board.squareSize,SCREEN_WIDTH, board.Rows * board.squareSize};
    // OnePlayer.RestartRect = {(SCREEN_WIDTH-board.squareSize)/2,(OnePlayer.playField.y-board.squareSize)/2,board.squareSize*2,board.squareSize*2};
    // OnePlayer.MenuRect = {OnePlayer.playField.y/4, OnePlayer.playField.y/4, OnePlayer.playField.y, OnePlayer.playField.y/2};
}

void drawOnePerson() {
    // OnePlayer.drawOnePlayer(board, renderer, MouseIsDown);
}

//----------------------------------------- menu-related ------------------------------------------

void menu_event_handling() {
    if (e.type == SDL_MOUSEBUTTONUP) {
        // int x,y;
        // SDL_GetMouseState(&x,&y);
        for (int i=0;i<listMenuSize;i++) {
            // SDL_Rect fillRect = {SCREEN_WIDTH/2 - maxWidth/2,topLeftY + i*(20 + maxHeight), maxWidth, maxHeight};
            if (isInSDLRect(MenuRects[i])) {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                SDL_RenderClear(renderer);
                switch (i) {
                    case Continue:
                        current_state = onePlayerScreen ;
                        break;
                    case NewGame :
                        current_state = onePlayerScreen ;
                        // OnePlayer.restart1p(board) ;
                        break;
                    case Settings :
                        current_state = settingScreen ;
                        chosenBackPic = 0;
                        break;
                    case Exit :
                        quit = true;
                        break;
                }
                break;
            }
        }
    }
}

void settingUpMenu() {
    int maxWidth = 0, maxHeight;
    for (int i=0;i<listMenuSize;i++) {
        maxWidth = max(maxWidth, listMenuTexture[i].width);
    }
    maxHeight = listMenuTexture[0].height*2;
    maxWidth += maxHeight;
    int topLeftX, topLeftY;
    if (listMenuSize%2==0) {
        topLeftY = SCREEN_HEIGHT/2 - (maxHeight + 20)*(listMenuSize/2)+10;
    } else {
        topLeftY = SCREEN_HEIGHT/2 - (maxHeight + 20)*(listMenuSize/2) - maxHeight/2;
    }
    for (int i=0;i<listMenuSize;i++) {
        MenuRects[i] = {SCREEN_WIDTH/2 - maxWidth/2,topLeftY + i*(20 + maxHeight), maxWidth, maxHeight};
    }
}

void drawingMenu() {
    SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
    for (int i=0;i<listMenuSize;i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &MenuRects[i]);
        listMenuTexture[i].render(MenuRects[i], renderer);
    }
}

//------------------------------------- setting-related ---------------------------------------

void setting_event_handling() {
    if (e.type == SDL_MOUSEBUTTONUP) {
        for (int i=0;i<2;i++) {
            for (int j=1;j<4;j++) {
                if (isInSDLRect(listSettingRects[i][j])) {
                    if (i==0) {
                        // OnePlayer.chosenDifficulty = j-1;
                        // OnePlayer.restart1p(board);
                    } else {
                        // OnePlayer.onePlayerChosenBackgroundX = j-1;
                    }
                }
            }
        }
        if (isInSDLRect(backRect)) {
            current_state = menuScreen ;
        }
    }
}

void settingUpSettings() {
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

void drawBackButton() {
    chosenBackPic = (chosenBackPic+1)%(backGifs*2);
    SDL_RenderCopy(renderer, backButton[chosenBackPic/2], NULL, &backRect);
}

void drawingSetting() {
    SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
    for (int i=0;i<2;i++) {
        int chosenNumber = 2;
        // if (i==0) chosenNumber = OnePlayer.chosenDifficulty;
        // if (i==1) chosenNumber = OnePlayer.onePlayerChosenBackgroundX;
        for (int j=0;j<4;j++) {
            if (j>0 && isInSDLRect(listSettingRects[i][j])) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else {
                if (j-1 == chosenNumber) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            SDL_RenderDrawRect(renderer, &listSettingRects[i][j]);
            listSettingTexture[i][j].render(listSettingRects[i][j], renderer);
        }
    }
    drawBackButton();
}

//------------------------------------- central-event-handling -----------------------------------------------

void event_handling() {
    if (current_state == onePlayerScreen) {
        // if (isInSDLRect(OnePlayer.playField)) {
        //     OnePlayer.board_event_handling(board, e);
        // }
        // else {
        //     if (isInSDLRect(OnePlayer.RestartRect)) {
        //         if (e.type == SDL_MOUSEBUTTONUP) OnePlayer.restart1p(board);
        //     }
        //     else if (isInSDLRect(OnePlayer.MenuRect)) {
        //         if (e.type == SDL_MOUSEBUTTONUP) {
        //             current_state = menuScreen;
        //             SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
        //             SDL_RenderClear(renderer);
        //         }
        //     }
        // }
    }
    else if (current_state == menuScreen) {
        menu_event_handling();
    }
    else if (current_state == settingScreen) {
        setting_event_handling();
    }
}

//------------------------------------------------- main ---------------------------------------------------


int main(int argc, char* argv[]) { // watch toaru pls :)
    srand(time(0));
    // cout << "fuck\n" ;
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // screen = SDL_GetWindowSurface(window);
    bool loadingSuccessfull = loadMedia();
    if (!loadingSuccessfull) {
        cout << "failed to load media" ;
        SDL_Delay(2000);
        quitSDL(window,renderer);
        return 0;
    }

    // Your drawing code here
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear( renderer );

    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

    //----------------------------------------------------------------------------------------------

    settingUpOnePerson();
    settingUpMenu();
    settingUpSettings();
    // OnePlayer.restart1p(board);
    // board.unReset();

    while (!quit) {
        if (SDL_PollEvent(&e)!=0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) MouseIsDown = true;
            if (e.type == SDL_MOUSEBUTTONUP) MouseIsDown = false;
            event_handling();
            
            // if (current_state == onePlayerScreen) OnePlayer();
        }
        switch (current_state) {
            case menuScreen:
                drawingMenu();
                break;
            case onePlayerScreen:
                drawOnePerson();
                break;
            case settingScreen:
                drawingSetting();
                break;
        }

        SDL_RenderPresent(renderer);
    }
    

    // waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    quitSDL(window, renderer);
    return 0;

}
