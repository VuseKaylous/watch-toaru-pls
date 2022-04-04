#include<iostream>
#include<ctime>
#include<SDL.h>
// #include<SDL_image.h>
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
SDL_Surface* screen ;
SDL_Texture* numbers[10] ;

SDL_Texture* loadSurface( std::string path )
{
    //The final optimized image
    SDL_Texture* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if( optimizedSurface == NULL )
        {
            printf( "Unable to create texture %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

bool loadMedia()
{
    bool success = true;
    //Load stretching surface
    string loadingPictures = "picture/x.bmp";
    for (char i='0';i<='9';i++) {
        loadingPictures[8] = i;
        numbers[i-'0'] = loadSurface(loadingPictures);
        if (numbers[i-'0'] == NULL) {
            // printf( "Failed to load image!\n" );
            cout << "Failed to load image " << i << "\n" ;
            success = false;
            break;
        }
    }

    return success;
}

//----------------------------------- real coding part -----------------------------------------------------------------------------------

//---------------------------------------- board-related -----------------------------------------


BOARD board;


void drawSquare(int x,int y, int w,int h, bool isCovered, SDL_Renderer* renderer,int cntBombs) {
    SDL_Rect fillRect = { x, y, w, h };
    if (isCovered) SDL_SetRenderDrawColor( renderer, 41, 223, 255, 0 );
    else {
        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 0 );
        
    }
    SDL_RenderFillRect( renderer, &fillRect );
    if (cntBombs>0 && !isCovered) {
        // cout << "fuck\n" ;
        SDL_RenderCopy( renderer, numbers[cntBombs], NULL, &fillRect );
    }
    // SDL_RenderPresent(renderer);
}

void BOARD::drawBoard(SDL_Renderer* renderer) {
    int squareSize = SCREEN_WIDTH/Cols;
    for (int i=0;i<Rows;i++) {
        for (int j=0;j<Cols;j++) {
            drawSquare(0+j*squareSize, SCREEN_HEIGHT - (Rows-i)*squareSize, squareSize,squareSize,cover[i][j], renderer, countBombs(i,j));
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



//------------------------------------------------- main ---------------------------------------------------


int main(int argc, char* argv[]) { // watch toaru pls :)
    srand(time(0));
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    screen = SDL_GetWindowSurface(window);
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
    // board.unReset();
    board.drawBoard(renderer);


    waitUntilKeyPressed();


    // use SDL_RenderPresent(renderer) to show it
    quitSDL(window, renderer);
    return 0;

}
