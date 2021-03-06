#ifndef RUSHIA_H
#define RUSHIA_H
#include<iostream>
#include<cstring>
#include<ctime>
#include<SDL.h>

using namespace std;


const int N = 100;
const int xi[]={1,1,1,0,0,-1,-1,-1}, yi[]={-1,0,1,-1,1,-1,0,1} ;


struct BOARD {
private:
    SDL_Texture *bomb, *flag, *trigerredBomb;
    SDL_Texture* numbers[10];
public:
    bool cover[N][N], isBomb[N][N], flagged[N][N]; // 10x10, 16x16, 30x16
    int Rows, Cols, squareSize, disFromTop, numNotBombs, trigerredX, trigerredY, difficulty;

    BOARD();
    ~BOARD();
    void BOARDfree();
    bool loadRushia(SDL_Renderer *renderer);
    void setDifficulty(int chosenDifficulty);
    void reset() ;
    void unReset() ;
    int countBombs(int x,int y) ;
    int countFlags(int x,int y) ;
    void floodField(int x,int y) ;
    bool floodField2(int x,int y) ;
    void drawSquare(int x,int y, SDL_Renderer* renderer, int xi,int yi, bool MouseDown, int mousePosX, int mousePosY);
    void drawBoard(SDL_Renderer* renderer, SDL_Rect playField, bool MouseDown, int mousePosX, int mousePosY) ;
};

#endif