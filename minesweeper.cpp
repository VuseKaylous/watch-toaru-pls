#include<iostream>
#include "SDL.h"
using namespace std;

void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 680;
const string WINDOW_TITLE = "Haachamachama!!!";

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (window == nullptr) logSDLError(std::cout, "CreateWindow", true);


    //Khi chạy trong môi trường bình thường (không chạy trong máy ảo)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);
    //Khi chạy ở máy ảo (ví dụ tại máy tính trong phòng thực hành ở trường)
    //renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));

    if (renderer == nullptr) logSDLError(std::cout, "CreateRenderer", true);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

//----------------------------------- real coding part ---------------------------------------

const int N = 100;
bool cover[N][N], isBomb[N][N]; // 10x10, 16x16, 30x16
int Rows, Cols;

void drawSquare(int x,int y, int w,int h, bool isCovered, SDL_Renderer* renderer) {
    SDL_Rect fillRect = { x, y, w, h };
    if (isCovered) SDL_SetRenderDrawColor( renderer, 95, 106, 122, 0 );
    else SDL_SetRenderDrawColor( renderer, 158, 182, 217, 0 );
    SDL_RenderFillRect( renderer, &fillRect );
    SDL_RenderPresent(renderer);
}

void reset() {
    memset(cover,0,sizeof(cover));
    memset(isBomb,0,sizeof(isBomb));
}

void drawBoard(SDL_Renderer* renderer) {
    int squareSize = SCREEN_WIDTH/cols;
    for (int i=0;i<Rows;i++) {
        for (int j=0;j<Cols;j++) {
            drawSquare(SCREEN_HEIGHT - (Rows-i)*squareSize, 0+j*squareSize, squareSize,squareSize,cover[i][j], renderer);
        }
    }
}

int countBombs(int x,int y) {
    int xi[]={1,1,1,0,0,-1,-1,-1}, yi[]={-1,0,1,-1,1,-1,0,1} ;
    int Haachamachama = 0;
    for (int i=0;i<8;i++) {
        if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) {
            Haachamachama += isBomb[x+xi[i]][y+yi[i]] ;
        }
    }
    return Haachamachama ;
}

int main(int argc, char* argv[]) {
	SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer);

    // Your drawing code here
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );
    // drawRect(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, renderer);


    // use SDL_RenderPresent(renderer) to show it

    waitUntilKeyPressed();
    quitSDL(window, renderer);
    return 0;

}
