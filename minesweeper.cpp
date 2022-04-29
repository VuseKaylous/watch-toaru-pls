#include<iostream>
#include<vector>
#include<ctime>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "sora.h"
#include "rushia.h"
#include "loadTexture.h"
#include "aloe.h"

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

enum png {bomb, flag, RestartButton, trigerredBomb, winning, menu, menuBackground} ;
string picChar[] = {"bomb", "flag", "RestartButton", "trigerredBomb", "winning", "menu", "menuBackground.jpg"};
int picCharSize = 7;
SDL_Texture* pic[7];

string onePlayerBackground[] = {"0.jpg", "1.jpg", "2.jpg", "3.png"};
int onePlayerBackgroundSize = 4, onePlayerChosenBackground;
SDL_Texture* onePlayerBackgroundTexture[4];

int backGifs = 41, chosenBackPic;
SDL_Texture* backButton[41];

enum screenState {menuScreen, onePlayerScreen, settingScreen};
int current_state = menuScreen;

bool MouseIsDown = false;
SDL_Texture* numbers[10];


enum listMenu {Continue, NewGame, Settings, Exit};
string listMenuName[] = {"Continue", "New game", "Settings", "Exit"} ;
int listMenuSize = 4;
LTexture listMenuTexture[4];
SDL_Rect MenuRects[4];

string listSettingName[1][4] = {"Difficulty:", "Easy", "Medium", "Hard"} ;
LTexture listSettingTexture[1][4];
SDL_Rect listSettingRects[1][4];
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
    //Load stretching surface
    string loadingPictures = "picture/SDL_image_related/";
    for (char i='0';i<='9';i++) {
        string si = loadingPictures + i + ".png";
        // loadingPictures[26] = i;
        numbers[i-'0'] = loadSurface(si, renderer);
        if (numbers[i-'0'] == NULL) {
            // printf( "Failed to load image!\n" );
            cout << "Failed to load image " << i << "\n" ;
            success = false;
            break;
        }
    }
    for (int i=0;i<7;i++) {
        string si;
        if (i<6) si = loadingPictures + picChar[i] + ".png";
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
        onePlayerBackgroundTexture[i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTexture[i] == NULL) {
            cout << "Failed to load image " << onePlayerBackground[i] << "\n" ;
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
        for (int i=0;i<4;i++) {
            if (!listSettingTexture[0][i].loadFromRenderedText(listSettingName[0][i], textColor, renderer, gFont)) {
                cout << "Failed to load texture " << listSettingName[0][i] << "\n" ;
                success = false;
                break;
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

    onePlayerChosenBackground = rand() % onePlayerBackgroundSize;

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
    SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackground], NULL, &playField);
    board.drawBoard(renderer, playField);
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
        for (int i=0;i<1;i++) {
            for (int j=1;j<4;j++) {
                if (isInSDLRect(listSettingRects[i][j])) {
                    chosenDifficulty = j-1;
                    restart1p();
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
    for (int i=0;i<1;i++) {
        for (int j=0;j<4;j++) {
            listSettingRects[i][j] = {x, y, listSettingTexture[i][j].width + listSettingTexture[i][j].height, listSettingTexture[i][j].height*2};
            x += listSettingRects[i][j].w + 30;
        }
        y += listSettingRects[i][0].h + 30 ;
    }
}

void drawingSetting() {
    SDL_RenderCopy(renderer, pic[menuBackground], NULL, NULL);
    for (int i=0;i<1;i++) {
        for (int j=0;j<4;j++) {
            if (j>0 && isInSDLRect(listSettingRects[i][j])) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else if (j-1 == chosenDifficulty) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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

//--------------------------------------------------------- board-related -----------------------------------------------------------

void BOARD::drawSquare(int x,int y, int w,int h, SDL_Renderer* renderer, int xi,int yi) {
    SDL_Rect fillRect = { x, y, w, h };
    if (cover[xi][yi]) {
        if (isIn(x,y,x+w,y+h)) { // hovering
            if (MouseIsDown) SDL_SetRenderDrawColor(renderer, 117, 202, 255, 255);
            else SDL_SetRenderDrawColor(renderer, 28, 149, 201, 255);
            SDL_RenderFillRect( renderer, &fillRect );
        }
        // else SDL_SetRenderDrawColor( renderer, 116, 150, 168, 255 );
        
        if (flagged[xi][yi]) {

            SDL_RenderCopy(renderer, pic[flag], NULL, &fillRect);
        }
    }
    else {
        if (isBomb[xi][yi]) {
            if (xi == trigerredX && yi == trigerredY) SDL_RenderCopy(renderer, pic[trigerredBomb], NULL,&fillRect);
            else SDL_RenderCopy(renderer, pic[bomb], NULL,&fillRect);
        }
        else {
            int cntBombs = countBombs(xi,yi);
            if (cntBombs>0) {
                SDL_RenderCopy( renderer, numbers[cntBombs], NULL, &fillRect );
            } else {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                SDL_RenderFillRect( renderer, &fillRect );
            }
        }
    }
}