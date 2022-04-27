#include<iostream>
#include<vector>
#include<ctime>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "sora.h"
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

enum png {bomb, flag, RestartButton, trigerredBomb, winning, menu} ;
string picChar[] = {"bomb", "flag", "RestartButton", "trigerredBomb", "winning", "menu"};
int picCharSize = 6;
SDL_Texture* pic[6];

enum screenState {menuScreen, onePlayerScreen};
int current_state = onePlayerScreen;

bool MouseIsDown = false;
SDL_Texture* numbers[10];


enum listMenu {NewGame, Exit};
string listMenuName[] = {"New game", "Exit"} ;
int listMenuSize = 2;
LTexture listMenuTexture[2];

SDL_Rect playField;
SDL_Rect RestartRect;
SDL_Rect MenuRect;

bool quit = false;
float winningOpacity = 0;
float winningShowUp = 0;

// time_t startTime,endTime;

//------------------------------------------ ¯\_(ツ)_/¯ ---------------------------------------------

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 ) {
            if (e.type == SDL_QUIT) return;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) return;
            }
        }
             // (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            // return;
        SDL_Delay(100);
    }
}

bool isIn(int x1,int y1,int x2,int y2) {
    int x,y;
    SDL_GetMouseState(&x,&y);
    if (x1<x && x<x2 && y1<y && y<y2) return true;
    return false;
}

bool isInSDLRect(SDL_Rect rect) {
    return isIn(rect.x, rect.y, rect.x+rect.w, rect.y + rect.h);
}

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
    for (int i=0;i<6;i++) {
        string si = loadingPictures + picChar[i] + ".png";
        pic[i] = loadSurface(si,renderer);
        if (pic[i] == NULL) {
            cout << "Failed to load image " << picChar[i] << "\n" ;
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
        SDL_Color textColor = { 0, 0, 0 };
        for (int i=0;i<listMenuSize;i++) {
            // listMenuTexture[i] = loadFromRenderedText(listMenuName[i], textColor, renderer, gFont);
            if (!listMenuTexture[i].loadFromRenderedText(listMenuName[i], textColor, renderer, gFont)) {
                cout << "Failed to load texture " << listMenuName[i] << "\n" ;
                success = false;
                break;
            }
        }
        // if( !gTextTexture.loadFromRenderedText( "The quick brown fox jumps over the lazy dog", textColor ) )
        // {
        //     printf( "Failed to render text texture!\n" );
        //     success = false;
        // }
    }

    return success;
}

//----------------------------------- real coding part -----------------------------------------------------------------------------------

void restart1p() {

    board.Rows = 27 ;
    board.Cols = 48 ;
    board.squareSize = SCREEN_WIDTH/board.Cols;
    // board.disFromTop = SCREEN_HEIGHT - board.Rows * board.squareSize;
    playField = {0,SCREEN_HEIGHT - board.Rows * board.squareSize,SCREEN_WIDTH, board.Rows * board.squareSize};
    // board.disFromTop = playField.y
    RestartRect = {(SCREEN_WIDTH-board.squareSize)/2,(playField.y-board.squareSize)/2,board.squareSize*2,board.squareSize*2};
    MenuRect = {playField.y/4, playField.y/4, playField.y, playField.y/2};
    board.reset();

    winningOpacity = 0;
    winningShowUp = 0;

    // startTime = time(0);

    // cout << board.numNotBombs << "\n" ;
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

void drawingMenu() {
    // gTextTexture.render( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2 );
    int maxWidth = 0;
    for (int i=0;i<listMenuSize;i++) {
        maxWidth = max(maxWidth, listMenuTexture[i].width);
    }
    int topLeftX, topLeftY;
    if (listMenuSize%2==0) {
        topLeftY = SCREEN_HEIGHT/2 - (listMenuTexture[0].height + 20)*(listMenuSize/2)+10;
    } else {
        topLeftY = SCREEN_HEIGHT/2 - (listMenuTexture[0].height + 20)*(listMenuSize/2) - listMenuTexture[0].height/2;
    }
    for (int i=0;i<listMenuSize;i++) {
        SDL_Rect fillRect = {SCREEN_WIDTH/2 - maxWidth/2,topLeftY + i*(20 + listMenuTexture[i].height), maxWidth, listMenuTexture[i].height};
        listMenuTexture[i].render(fillRect, renderer);
    }
}

void event_handling() {
    if (current_state == onePlayerScreen) {
        if (isInSDLRect(playField)) {
            board_event_handling();
        }
        else {
            if (isInSDLRect(RestartRect)) {
                if (MouseIsDown) restart1p();
            }
            else if (isInSDLRect(MenuRect)) {
                if (MouseIsDown) {
                    current_state = menuScreen;
                    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                    SDL_RenderClear(renderer);
                }
            }
        }
    }
    else {

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
        waitUntilKeyPressed();
        quitSDL(window,renderer);
        return 0;
    }

    // Your drawing code here
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear( renderer );

    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

    //----------------------------------------------------------------------------------------------


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
            switch (current_state) {
                case menuScreen:
                    drawingMenu();
                    break;
                case onePlayerScreen:
                    OnePlayer();
                    break;
                
            }
            // if (current_state == onePlayerScreen) OnePlayer();
        }

        SDL_RenderPresent(renderer);
    }
    

    // waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    quitSDL(window, renderer);
    return 0;

}

//---------------------------------------- board-related -----------------------------------------

void BOARD::drawSquare(int x,int y, int w,int h, SDL_Renderer* renderer, int xi,int yi) {
    SDL_Rect fillRect = { x, y, w, h };
    if (cover[xi][yi]) {
        if (isIn(x,y,x+w,y+h)) { // hovering
            if (MouseIsDown) SDL_SetRenderDrawColor(renderer, 117, 202, 255, 255);
            else SDL_SetRenderDrawColor(renderer, 28, 149, 201, 255);
        }
        else SDL_SetRenderDrawColor( renderer, 116, 150, 168, 255 );
        SDL_RenderFillRect( renderer, &fillRect );
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