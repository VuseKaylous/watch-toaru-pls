#include<iostream>
#include<ctime>
#include<SDL.h>
#include<SDL_image.h>
// #include "SDL_ttf.h"
#include "sora.h"
#include "rushia.h"

using namespace std;

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 680;
const string WINDOW_TITLE = "Haachamachama!!!";

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

//----------------------------------------- SDL preparations -------------------------------------------

SDL_Window* window;
SDL_Renderer* renderer;
// SDL_Surface* screen ;
SDL_Event e;
// SDL_MouseButtonEvent Rat;
bool MouseIsDown = false;

SDL_Texture* numbers[10];
SDL_Texture* bomb ;
SDL_Texture* flag ;
SDL_Texture* RestartButton ;
SDL_Texture* trigerredBomb ;
SDL_Texture* winning;
SDL_Texture* menu;


BOARD board;

SDL_Rect MenuRect;

bool quit = false;
float winningOpacity = 0;
float winningShowUp = 0;

// time_t startTime,endTime;

bool loadMedia()
{
    bool success = true;
    //Load stretching surface
    string loadingPictures = "picture/SDL_image_related/x.png";
    for (char i='0';i<='9';i++) {
        loadingPictures[26] = i;
        numbers[i-'0'] = loadSurface(loadingPictures, renderer);
        if (numbers[i-'0'] == NULL) {
            // printf( "Failed to load image!\n" );
            cout << "Failed to load image " << i << "\n" ;
            success = false;
            break;
        }
    }
    bomb = loadSurface("picture/SDL_image_related/bomb.png",renderer);
    if (bomb == NULL) {
        cout << "Failed to load bomb " << "\n" ;
        success = false;
    }
    flag = loadSurface("picture/SDL_image_related/flag.png",renderer);
    if (flag == NULL) {
        cout << "Failed to load flag " << "\n" ;
        success = false;
    }
    RestartButton = loadSurface("picture/SDL_image_related/restartButton.png",renderer);
    if (RestartButton == NULL) {
        cout << "Failed to load restart button " << "\n" ;
        success = false;
    }
    trigerredBomb = loadSurface("picture/SDL_image_related/trigerredBomb.png",renderer);
    if (trigerredBomb == NULL) {
        cout << "Failed to load trigerred bomb " << "\n" ;
        success = false;
    }
    winning = loadSurface("picture/SDL_image_related/winning.png",renderer);
    if (winning == NULL) {
        cout << "Failed to load winning " << "\n" ;
        success = false;
    }
    menu = loadSurface("picture/SDL_image_related/menu.png",renderer);
    if (menu == NULL) {
        cout << "Failed to load menu " << "\n" ;
        success = false;
    }

    return success;
}

//----------------------------------- real coding part -----------------------------------------------------------------------------------

bool isIn(int x1,int y1,int x2,int y2) {
    int x,y;
    SDL_GetMouseState(&x,&y);
    if (x1<x && x<x2 && y1<y && y<y2) return true;
    return false;
}

void restart1p(SDL_Rect &playField, SDL_Rect &RestartRect) {

    board.Rows = 24 ;
    board.Cols = 30 ;
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
            SDL_RenderCopy(renderer, flag, NULL, &fillRect);
        }
    }
    else {
        if (isBomb[xi][yi]) {
            if (xi == trigerredX && yi == trigerredY) SDL_RenderCopy(renderer, trigerredBomb, NULL,&fillRect);
            else SDL_RenderCopy(renderer, bomb, NULL,&fillRect);
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

//--------------------------------------------- event-related ---------------------------------------------------

bool restartOrNot(SDL_Rect RestartRect, int x,int y) {
    int x1,x2,y1,y2;
    x1 = RestartRect.x;
    y1 = RestartRect.y;
    x2 = x1 + RestartRect.w;
    y2 = y1 + RestartRect.h;
    return (x1<=x && x<=x2 && y1<=y && y<=y2);
}



void Solve(SDL_Rect &RestartRect, SDL_Rect &playField) {
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
                
        } else {
            if (restartOrNot(RestartRect,x,y)) restart1p(playField, RestartRect);
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

void OnePlayer(SDL_Rect &RestartRect, SDL_Rect &playField) {
    board.drawBoard(renderer, playField);
    SDL_RenderCopy(renderer, RestartButton, NULL, &RestartRect);
    SDL_RenderCopy(renderer, menu, NULL, &MenuRect);

    if (board.numNotBombs <= 0 && winningShowUp == 0) winningShowUp = 5;

    if (winningShowUp!=0) {
        if (winningShowUp > 0 && winningOpacity < 255) winningOpacity += winningShowUp;
        if (winningShowUp < 0 && winningOpacity > 0) winningOpacity += winningShowUp;
        SDL_SetTextureAlphaMod(winning, winningOpacity);
        SDL_RenderCopy(renderer, winning, NULL, &playField);
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


    SDL_Rect RestartRect;
    SDL_Rect playField;

    restart1p(playField, RestartRect);
    // board.unReset();

    while (!quit) {
        if (SDL_PollEvent(&e)!=0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) MouseIsDown = true;
            if (e.type == SDL_MOUSEBUTTONUP) MouseIsDown = false;
            Solve(RestartRect, playField);
        }

        OnePlayer(RestartRect, playField);

        SDL_RenderPresent(renderer);
    }
    

    // waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    quitSDL(window, renderer);
    return 0;

}
