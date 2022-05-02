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
int chosenDifficulty = 1;

enum png {RestartButton, winning, menu, menuBackground} ;
string picChar[] = {"RestartButton", "winning", "menu", "menuBackground.jpg"};
int picCharSize = 4;
SDL_Texture* pic[4];

string onePlayerBackground[] = {"0.jpg", "1.jpg", "2.jpg", "3.png"};
int onePlayerBackgroundSize = 4, onePlayerChosenBackgroundX, onePlayerChosenBackgroundY;
SDL_Texture* onePlayerBackgroundTexture[2][4];
SDL_Texture* onePlayerBackgroundTextureGif[39];

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

SDL_Rect playField;
SDL_Rect RestartRect;
SDL_Rect MenuRect;

bool quit = false;
float winningOpacity = 0;
float winningShowUp = 0;

// time_t startTime,endTime;

//------------------------------------------ ¯\_(ツ)_/¯ ---------------------------------------------

bool restartOrNot(int x,int y) {
    int x1,x2,y1,y2;
    x1 = RestartRect.x;
    y1 = RestartRect.y;
    x2 = x1 + RestartRect.w;
    y2 = y1 + RestartRect.h;
    return (x1<=x && x<=x2 && y1<=y && y<=y2);
}

//----------------------------------------- SDL preparations -------------------------------------------

bool loadMedia()
{
    bool success = true;
    bool checkSuccess = board.loadRushia(renderer);
    if (!checkSuccess) success = false;

    string loadingPictures = "picture/SDL_image_related/";
    for (int i=0;i<picCharSize;i++) {
        string si;
        if (i<picCharSize-1) si = loadingPictures + picChar[i] + ".png";
        else si = loadingPictures + picChar[i] ;
        pic[i] = loadSurface(si,renderer);
        if (pic[i] == NULL) {
            cout << "Failed to load image " << picChar[i] << "\n" ;
            success = false;
            break;
        }
    }
    loadingPictures = "picture/background_material/AH/";
    for (int i=0;i<onePlayerBackgroundSize;i++) {
        string si;
        si = loadingPictures + onePlayerBackground[i] ;
        onePlayerBackgroundTexture[0][i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTexture[0][i] == NULL) {
            cout << "Failed to load image AH" << onePlayerBackground[i] << "\n" ;
            success = false;
            break;
        }
    }
    loadingPictures = "picture/background_material/SS/";
    for (int i=0;i<onePlayerBackgroundSize;i++) {
        string si;
        si = loadingPictures + onePlayerBackground[i] ;
        onePlayerBackgroundTexture[1][i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTexture[1][i] == NULL) {
            cout << "Failed to load image SS" << onePlayerBackground[i] << "\n" ;
            success = false;
            break;
        }
    }
    loadingPictures = "picture/background_material/SS_gif/gifs/";
    for (int i=0;i<39;i++) {
        string si;
        si = loadingPictures + toString(i) + ".gif" ;
        onePlayerBackgroundTextureGif[i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTextureGif[i] == NULL) {
            cout << "Failed to load image background gif" << i << "\n" ;
            success = false;
            break;
        }
    }
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

void restart1p() {
    board.setDifficulty(chosenDifficulty);
    board.squareSize = SCREEN_WIDTH/board.Cols;
    // board.disFromTop = SCREEN_HEIGHT - board.Rows * board.squareSize;
    playField = {0,SCREEN_HEIGHT - board.Rows * board.squareSize,SCREEN_WIDTH, board.Rows * board.squareSize};
    // board.disFromTop = playField.y
    RestartRect = {(SCREEN_WIDTH-board.squareSize)/2,(playField.y-board.squareSize)/2,board.squareSize*2,board.squareSize*2};
    MenuRect = {playField.y/4, playField.y/4, playField.y, playField.y/2};
    board.reset();

    onePlayerChosenBackgroundY = rand() % onePlayerBackgroundSize;

    winningOpacity = 0;
    winningShowUp = 0;

    // startTime = time(0);

    // cout << board.numNotBombs << "\n" ;
}

void drawBackButton() {
    chosenBackPic = (chosenBackPic+1)%(backGifs*2);
    SDL_RenderCopy(renderer, backButton[chosenBackPic/2], NULL, &backRect);
}


//--------------------------------------------- event-related ---------------------------------------------------

//------------------------------------ one-player-related --------------------------------------

void board_event_handling() {
    if (e.type == SDL_MOUSEBUTTONUP) {
        int x,y;
        SDL_GetMouseState(&x,&y);
        if (y >= playField.y) { // inside board
            if (winningShowUp == 0) {
                int xi = y - playField.y,yi=x;
                xi/=board.squareSize;
                yi/=board.squareSize;
                // cout << xi << " " << yi << "\n" ;
                if (board.cover[xi][yi]) {
                    // cout << Rat.button << "\n";
                    if (e.button.button == SDL_BUTTON_LEFT && !board.flagged[xi][yi]) {
                        if (!board.isBomb[xi][yi]) {
                            if (board.countBombs(xi,yi)==0) board.floodField(xi,yi);
                            else {
                                board.cover[xi][yi] = false;
                                board.numNotBombs--;
                            }
                        }
                        else {
                            board.trigerredX = xi;
                            board.trigerredY = yi;
                            board.unReset();
                        }
                    }
                    if (e.button.button == SDL_BUTTON_RIGHT && board.cover[xi][yi]) board.flagged[xi][yi] = 1 - board.flagged[xi][yi];
                }
            } else {
                winningShowUp = 0-winningShowUp;
            }
        }
    }
    if (e.button.clicks == 2) {
        // cout << "fuck " ;
        int x,y;
        SDL_GetMouseState(&x,&y);
        int xi = y - playField.y,yi=x;
        xi/=board.squareSize;
        yi/=board.squareSize;
        // cout << xi << " " << yi << " " << board.cover[xi][yi] << " " << board.countBombs(xi,yi) << " " << board.countFlags(xi,yi) << "\n" ;
        if (!board.cover[xi][yi] && board.countBombs(xi,yi) == board.countFlags(xi,yi)) {
            // cout << "possible \n" ;
            // board.cover[xi][yi] = true;
            bool test = board.floodField2(xi,yi);
            if (!test) board.unReset();
        }
    }
}

void OnePlayer() {
    // SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY/2], NULL, &playField);
    if (onePlayerChosenBackgroundX == 2) {
        SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY/4], NULL, &playField);
        onePlayerChosenBackgroundY = (onePlayerChosenBackgroundY + 1)%(39*4);
    } else {
        SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY], NULL, &playField);
    }
    board.drawBoard(renderer, playField, MouseIsDown);
    SDL_RenderCopy(renderer, pic[RestartButton], NULL, &RestartRect);
    SDL_RenderCopy(renderer, pic[menu], NULL, &MenuRect);

    if (board.numNotBombs <= 0 && winningShowUp == 0) winningShowUp = 5;

    if (winningShowUp!=0) {
        if (winningShowUp > 0 && winningOpacity < 255) winningOpacity += winningShowUp;
        if (winningShowUp < 0 && winningOpacity > 0) winningOpacity += winningShowUp;
        SDL_SetTextureAlphaMod(pic[winning], winningOpacity);
        SDL_RenderCopy(renderer, pic[winning], NULL, &playField);
    }
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
                        restart1p() ;
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
    SDL_RenderCopy(renderer, pic[menuBackground], NULL, NULL);
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
                        chosenDifficulty = j-1;
                        restart1p();
                    } else {
                        onePlayerChosenBackgroundX = j-1;
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

void drawingSetting() {
    SDL_RenderCopy(renderer, pic[menuBackground], NULL, NULL);
    for (int i=0;i<2;i++) {
        int chosenNumber ;
        if (i==0) chosenNumber = chosenDifficulty;
        if (i==1) chosenNumber = onePlayerChosenBackgroundX;
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
        if (isInSDLRect(playField)) {
            board_event_handling();
        }
        else {
            if (isInSDLRect(RestartRect)) {
                if (e.type == SDL_MOUSEBUTTONUP) restart1p();
            }
            else if (isInSDLRect(MenuRect)) {
                if (e.type == SDL_MOUSEBUTTONUP) {
                    current_state = menuScreen;
                    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                    SDL_RenderClear(renderer);
                }
            }
        }
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
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // screen = SDL_GetWindowSurface(window);
    bool loadingSuccessfull = loadMedia();
    if (!loadingSuccessfull) {
        cout << "failed to load media" ;
        SDL_Delay(100);
        quitSDL(window,renderer);
        return 0;
    }

    // Your drawing code here
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear( renderer );

    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

    //----------------------------------------------------------------------------------------------

    chosenDifficulty = 1;
    onePlayerChosenBackgroundX = 0;
    settingUpMenu();
    settingUpSettings();
    restart1p();
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
                OnePlayer();
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
