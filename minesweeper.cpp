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


bool quit = false;

bool loadMedia()
{
    bool success = true;
    //Load stretching surface
    string loadingPictures = "picture/x.bmp";
    for (char i='0';i<='9';i++) {
        loadingPictures[8] = i;
        numbers[i-'0'] = loadSurface(loadingPictures, renderer);
        if (numbers[i-'0'] == NULL) {
            // printf( "Failed to load image!\n" );
            cout << "Failed to load image " << i << "\n" ;
            success = false;
            break;
        }
    }
    bomb = loadSurface("picture/bomb.bmp",renderer);
    if (bomb == NULL) {
        cout << "Failed to load bomb " << "\n" ;
        success = false;
    }
    flag = loadSurface("picture/flag.bmp",renderer);
    if (flag == NULL) {
        cout << "Failed to load flag " << "\n" ;
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

//---------------------------------------- board-related -----------------------------------------


BOARD board;

void drawSquare(int x,int y, int w,int h, bool isCovered, SDL_Renderer* renderer,int cntBombs, int xi,int yi) {
    SDL_Rect fillRect = { x, y, w, h };
    if (isCovered) {
        if (isIn(x,y,x+w,y+h)) {
            if (MouseIsDown){
                SDL_SetRenderDrawColor(renderer, 117, 202, 255, 0);
            }
            else SDL_SetRenderDrawColor(renderer, 28, 149, 201, 0);
        }
        else SDL_SetRenderDrawColor( renderer, 116, 150, 168, 0 );
        SDL_RenderFillRect( renderer, &fillRect );
        if (board.flagged[xi][yi]) {
            SDL_RenderCopy(renderer, flag, NULL, &fillRect);
        }
    }
    else {
        if (board.isBomb[xi][yi]) SDL_RenderCopy(renderer, bomb, NULL,&fillRect);
        else {
            if (cntBombs>0) {
                SDL_RenderCopy( renderer, numbers[cntBombs], NULL, &fillRect );
            } else {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 0 );
                SDL_RenderFillRect( renderer, &fillRect );
            }
        }
    }
    
}

void BOARD::drawBoard(SDL_Renderer* renderer) {
    // squareSize = SCREEN_WIDTH/Cols;
    for (int i=0;i<Rows;i++) {
        for (int j=0;j<Cols;j++) {
            drawSquare(0+j*squareSize, SCREEN_HEIGHT - (Rows-i)*squareSize, squareSize,squareSize,cover[i][j], renderer, countBombs(i,j), i,j);
            // cout << countBombs(i,j) << "\n" ;
        }
    }
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    for (int i=0;i<=Rows;i++) {
        SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT-(Rows-i)*squareSize, SCREEN_WIDTH, SCREEN_HEIGHT-(Rows-i)*squareSize);
    }
    for (int i=0;i<=Cols;i++) {
        SDL_RenderDrawLine(renderer, i*squareSize, SCREEN_HEIGHT-Rows*squareSize, i*squareSize, SCREEN_HEIGHT );
    }

    SDL_UpdateWindowSurface( window );
    SDL_RenderPresent(renderer);
    
    // SDL_RenderDrawLine( renderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );
}
//--------------------------------------------- event-related ---------------------------------------------------

void Solve() {
    if (e.type == SDL_MOUSEBUTTONUP) {
        int x,y;
        SDL_GetMouseState(&x,&y);
        int xi = y - board.disFromTop,yi=x;
        xi/=board.squareSize;
        yi/=board.squareSize;
        // cout << xi << " " << yi << "\n" ;
        if (board.cover[xi][yi]) {
            // cout << Rat.button << "\n";
            if (e.button.button == SDL_BUTTON_LEFT && !board.flagged[xi][yi]) {
                if (!board.isBomb[xi][yi]) {
                    if (board.countBombs(xi,yi)==0) board.floodField(xi,yi);
                    else board.cover[xi][yi] = false;
                }
                else {
                    board.unReset();
                }
            }
            if (e.button.button == SDL_BUTTON_RIGHT && board.cover[xi][yi]) board.flagged[xi][yi] = 1 - board.flagged[xi][yi];
        }
    }
    if (e.button.clicks == 2) {
        // cout << "fuck " ;
        int x,y;
        SDL_GetMouseState(&x,&y);
        int xi = y - board.disFromTop,yi=x;
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
    // drawRect(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, renderer);

    board.Rows = 24 ;
    board.Cols = 30 ;
    board.reset();
    board.squareSize = SCREEN_WIDTH/board.Cols;
    board.disFromTop = SCREEN_HEIGHT - board.Rows * board.squareSize;
    // board.unReset();
    while (!quit) {
        if (SDL_PollEvent(&e)!=0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) MouseIsDown = true;
            if (e.type == SDL_MOUSEBUTTONUP) MouseIsDown = false;
            Solve();
        }
        board.drawBoard(renderer);
    }
    

    // waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    quitSDL(window, renderer);
    return 0;

}
