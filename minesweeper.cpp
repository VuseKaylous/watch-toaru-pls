#include<iostream>
#include<fstream>
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
#include "setting.h"
#include "menu.h"
#include "cursor.h"

using namespace std;

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 640;
const string WINDOW_TITLE = "Haachamachama!!!";

SDL_Window* window;
SDL_Renderer* renderer;
// SDL_Surface* screen ;
SDL_Event e;
TTF_Font *gFont = NULL;
bool MouseIsDown = false;

SDL_Texture *mainScreen, *presentScreen;

MENU Menu;

SDL_Texture *Background;

enum screenState {menuScreen, onePlayerScreen, settingScreen};
int current_state = menuScreen;

bool quit = false;

int *savedSetting[4];

// time_t startTime,endTime;


//----------------------------------------- SDL preparations -------------------------------------------

bool loadMedia()
{
    bool success = true;

    gFont = TTF_OpenFont( "fonts/lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
        return success;
    }

    bool checkSuccess = true;

    // cout << "fuck\n" ;
    checkSuccess = Menu.Setting.loadSetting(renderer, gFont);
    if (!checkSuccess) success = false;
    checkSuccess = Menu.loadMenu(renderer, gFont);
    if (!checkSuccess) success = false;

    string loadingPictures;

    loadingPictures = "picture/SDL_image_related/menuBackground.jpg";
    Background = loadSurface(loadingPictures, renderer);
    if (Background == NULL) success = false;

    return success;
}

//------------------------------------------ One-person-related ------------------------------------

void settingUpOnePerson() {
    Menu.Setting.OnePlayer.settingUp1p(SCREEN_WIDTH, SCREEN_HEIGHT);
    Menu.Setting.OnePlayer.miniScreen.setSubScreen(Menu.Setting.chosenMouseMode);
}

//----------------------------------------- menu-related ------------------------------------------

void settingUpMenu() {
    Menu.menuBackground = Background;
    Menu.settingUpMenu_Menu(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
}

//------------------------------------- setting-related -------------------------------------------

void settingUpSettingMain() {
    Menu.Setting.settingUpSettings();
    Menu.Setting.backRect = {SCREEN_WIDTH - 200, SCREEN_HEIGHT - 200, 200, 200};
    Menu.Setting.settingBackground = Background;
}

//------------------------------------- central-event-handling -----------------------------------------------------

void event_handling() {
    if (current_state == onePlayerScreen) {
        Menu.Setting.OnePlayer.onePlayer_event_handling(e, current_state);
    }
    else if (current_state == menuScreen) {
        if (Menu.menu_event_handling(e, current_state, renderer)) quit = true;
    }
    else if (current_state == settingScreen) {
        bool check = Menu.Setting.setting_event_handling(e);
        if (check) current_state = menuScreen;
    }
}

//------------------------------------------------------------ real coding part ----------------------------------------------------------

void settingUpEverything() {
    ifstream inp("savedSettings.txt");
    // for (int i=0;i<3;i++) inp >> savedSetting[i] ;
    inp >> Menu.Setting.OnePlayer.chosenDifficulty >> Menu.Setting.OnePlayer.onePlayerChosenBackgroundX >> Menu.Setting.OnePlayer.mouse.usingCursor >> Menu.Setting.chosenMouseMode ;
    inp.close();

    settingUpOnePerson();
    settingUpMenu();
    settingUpSettingMain();

    savedSetting[0] = &Menu.Setting.OnePlayer.chosenDifficulty ;
    savedSetting[1] = &Menu.Setting.OnePlayer.onePlayerChosenBackgroundX ;
    savedSetting[2] = &Menu.Setting.OnePlayer.mouse.usingCursor;
    savedSetting[3] = &Menu.Setting.chosenMouseMode;
    Menu.Setting.OnePlayer.mouse.setCursor();

    // OnePlayer.restart1p(board);
}

void drawEverything() {
    mainScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, mainScreen);

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear(renderer);

    switch (current_state) {
        case menuScreen:
            Menu.drawingMenu(renderer);
            break;
        case onePlayerScreen:
            Menu.Setting.OnePlayer.drawOnePlayer(renderer, MouseIsDown);
            break;
        case settingScreen:
            Menu.Setting.drawingSetting(renderer);
            break;
    }
    SDL_SetRenderTarget(renderer, presentScreen);
    if (current_state == onePlayerScreen && Menu.Setting.chosenMouseMode == 1) {
        SDL_RenderCopy(renderer, mainScreen, &Menu.Setting.OnePlayer.miniScreen.boingboing, NULL);
    } else {
        SDL_RenderCopy(renderer, mainScreen, NULL, NULL);
    }
    Menu.Setting.OnePlayer.mouse.drawCursor(renderer);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void graduate() {
    ofstream output("savedSettings.txt", ios::out | ios::trunc);
    for (int i=0;i<4;i++) output << *savedSetting[i] << " " ;
    output.close();

    Menu.MENUfree();
    Menu.Setting.SETTINGfree();
    quitSDL(window, renderer);
}

//------------------------------------------------- main ---------------------------------------------------


int main(int argc, char* argv[]) { // watch toaru pls :)
    srand(time(0));
    cout << "fuck\n" ;
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // screen = SDL_GetWindowSurface(window);
    bool loadingSuccessfull = loadMedia();
    if (!loadingSuccessfull) {
        cout << "failed to load media" ;
        // SDL_Delay(2000);
        quitSDL(window,renderer);
        return 0;
    }

    // Your drawing code here
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear( renderer );

    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

    //----------------------------------------------------------------------------------------------

    settingUpEverything();

    // miniScreen = {0,0,300,300};

    while (!quit) {
        Menu.Setting.OnePlayer.mouse.getExactPos(Menu.Setting.OnePlayer.miniScreen);
        drawEverything();

        if (SDL_PollEvent(&e)!=0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                if (e.key.keysym.sym == SDLK_3) {
                    Menu.Setting.OnePlayer.chosenDifficulty = 3;
                    Menu.Setting.OnePlayer.board.setDifficulty(3);
                    Menu.Setting.OnePlayer.restart1p();
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) MouseIsDown = true;
            if (e.type == SDL_MOUSEBUTTONUP) MouseIsDown = false;

            event_handling();
        }
        Menu.Setting.OnePlayer.mouse.cursor_event_handling();
        if (current_state == onePlayerScreen) Menu.Setting.OnePlayer.miniScreen.subScreen_event_handling(Menu.Setting.OnePlayer.mouse.realPosX, Menu.Setting.OnePlayer.mouse.realPosY);
        
    }
    

    // waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    graduate();
    return 0;

}
