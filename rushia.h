#include<iostream>
#include<cstring>
#include<ctime>
#include<SDL.h>

using namespace std;


const int N = 100;

struct BOARD {
    bool cover[N][N], isBomb[N][N]; // 10x10, 16x16, 30x16
    int Rows, Cols;

    void reset() {
        // memset(cover,0,sizeof(cover));
        memset(isBomb,0,sizeof(isBomb));
        for (int i=0;i<Rows;i++) for (int j=0;j<Cols;j++) cover[i][j] = true;
        int numBombs = Rows*Cols/5;
        int notBombs = Rows*Cols - numBombs;
        for (int i=0;i<Rows;i++) {
            for (int j=0;j<Cols;j++) {
                int x = rand() % (numBombs+notBombs);
                if (x<numBombs) {
                    numBombs--;
                    isBomb[i][j] = true;
                } else notBombs--;
            }
        }
    }
    void unReset() {
    	for (int i=0;i<Rows;i++) for (int j=0;j<Cols;j++) cover[i][j] = false;
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
    void drawBoard(SDL_Renderer* renderer);
};