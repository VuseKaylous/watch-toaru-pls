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

ONEPLAYER OnePlayer;
SETTING Setting;
MENU Menu;
CURSOR mouse;

SDL_Texture *Background;

enum screenState {menuScreen, onePlayerScreen, settingScreen};
int current_state = menuScreen;

bool quit = false;

int *savedSetting[3];

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

    bool checkSuccess = OnePlayer.board.loadRushia(renderer);
    if (!checkSuccess) success = false;
    // cout << "fuck\n" ;
    checkSuccess = OnePlayer.loadOnePlayer(renderer, gFont);
    if (!checkSuccess) success = false;
    checkSuccess = Setting.loadSetting(renderer, gFont);
    if (!checkSuccess) success = false;
    checkSuccess = Menu.loadMenu(renderer, gFont);
    if (!checkSuccess) success = false;
    checkSuccess = mouse.loadCursor(renderer);
    if (!checkSuccess) success = false;

    string loadingPictures;

    loadingPictures = "picture/SDL_image_related/menuBackground.jpg";
    Background = loadSurface(loadingPictures, renderer);
    if (Background == NULL) success = false;

    return success;
}

//------------------------------------------ One-person-related ------------------------------------

void settingUpOnePerson() {
    OnePlayer.board.squareSize = SCREEN_WIDTH/OnePlayer.board.Cols;
    OnePlayer.playField = {0,SCREEN_HEIGHT - OnePlayer.board.Rows * OnePlayer.board.squareSize,SCREEN_WIDTH, OnePlayer.board.Rows * OnePlayer.board.squareSize};

    // OnePlayer.RestartRect = {(SCREEN_WIDTH-board.squareSize)/2,(OnePlayer.playField.y-board.squareSize)/2,board.squareSize*2,board.squareSize*2};
    OnePlayer.RestartRect.w = OnePlayer.RestartRect.h = OnePlayer.playField.y/2;
    OnePlayer.RestartRect.x = SCREEN_WIDTH/2 - OnePlayer.RestartRect.w/2;
    OnePlayer.RestartRect.y = OnePlayer.RestartRect.h/2;

    OnePlayer.MenuRect = {OnePlayer.playField.y/4, OnePlayer.playField.y/4, OnePlayer.playField.y, OnePlayer.playField.y/2};
    OnePlayer.MenuRect.w = OnePlayer.MenuRect.h / OnePlayer.menu.height * OnePlayer.menu.width + OnePlayer.menu.height;
    // cout << OnePlayer.MenuRect.x << " " << OnePlayer.MenuRect.y << " " << OnePlayer.MenuRect.w << " " << OnePlayer.MenuRect.h << "\n" ;
    OnePlayer.walfieRect = {SCREEN_WIDTH - OnePlayer.playField.y/2*3, 0, OnePlayer.playField.y/2*3 , OnePlayer.playField.y};
    OnePlayer.walfieMovingRect = OnePlayer.walfieRect;
}

//----------------------------------------- menu-related ------------------------------------------

void settingUpMenu() {
    Menu.menuBackground = Background;
    Menu.settingUpMenu_Menu(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
}

//------------------------------------- setting-related -------------------------------------------

void settingUpSettingMain() {
    Setting.settingUpSettings();
    Setting.backRect = {SCREEN_WIDTH - 200, SCREEN_HEIGHT - 200, 200, 200};
    Setting.settingBackground = Background;
}

//------------------------------------- central-event-handling -----------------------------------------------------

void event_handling() {
    if (current_state == onePlayerScreen) {
        if (isInSDLRect(OnePlayer.playField)) {
            OnePlayer.board_event_handling(e);
        }
        else if (isInSDLRect(OnePlayer.RestartRect)) {
            if (e.type == SDL_MOUSEBUTTONUP) OnePlayer.restart1p();
        }
        else if (isInSDLRect(OnePlayer.MenuRect)) {
            if (e.type == SDL_MOUSEBUTTONUP) {
                current_state = menuScreen;
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                // SDL_RenderClear(renderer);
            }
        }
    }
    else if (current_state == menuScreen) {
        if (Menu.menu_event_handling(e, current_state, renderer, OnePlayer, Setting)) quit = true;
    }
    else if (current_state == settingScreen) {
        bool check = Setting.setting_event_handling(e, OnePlayer, mouse);
        if (check) current_state = menuScreen;
    }
}

//------------------------------------------------------------ real coding part ----------------------------------------------------------

void settingUpEverything() {
    ifstream inp("savedSettings.txt");
    // for (int i=0;i<3;i++) inp >> savedSetting[i] ;
    inp >> OnePlayer.chosenDifficulty >> OnePlayer.onePlayerChosenBackgroundX >> mouse.usingCursor ;
    inp.close();

    settingUpOnePerson();
    settingUpMenu();
    settingUpSettingMain();

    savedSetting[0] = &OnePlayer.chosenDifficulty ;
    savedSetting[1] = &OnePlayer.onePlayerChosenBackgroundX ;
    savedSetting[2] = &mouse.usingCursor;
    mouse.setCursor();

    // OnePlayer.restart1p(board);
}

void graduate() {
    ofstream output("savedSettings.txt", ios::out | ios::trunc);
    for (int i=0;i<3;i++) output << *savedSetting[i] << " " ;
    output.close();

    mouse.CURSORfree();
    Menu.MENUfree();
    Setting.SETTINGfree();
    OnePlayer.ONEPLAYERfree();
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

    while (!quit) {
        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
        SDL_RenderClear(renderer);
        if (SDL_PollEvent(&e)!=0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                if (e.key.keysym.sym == SDLK_3) {
                    OnePlayer.chosenDifficulty = 3;
                    OnePlayer.board.setDifficulty(3);
                    OnePlayer.restart1p();
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) MouseIsDown = true;
            if (e.type == SDL_MOUSEBUTTONUP) MouseIsDown = false;

            event_handling();
        }
        mouse.cursor_event_handling();
        switch (current_state) {
            case menuScreen:
                Menu.drawingMenu(renderer);
                break;
            case onePlayerScreen:
                OnePlayer.drawOnePlayer(renderer, MouseIsDown);
                break;
            case settingScreen:
                Setting.drawingSetting(renderer, OnePlayer, mouse);
                break;
        }
        mouse.drawCursor(renderer);

        SDL_RenderPresent(renderer);
    }
    

    // waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    graduate();
    return 0;

}
