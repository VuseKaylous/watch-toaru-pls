#include<iostream>
#include<cstring>
#include<ctime>
#include<SDL.h>

using namespace std;


const int N = 100;
const int xi[]={1,1,1,0,0,-1,-1,-1}, yi[]={-1,0,1,-1,1,-1,0,1} ;

struct BOARD {
    bool cover[N][N], isBomb[N][N], flagged[N][N]; // 10x10, 16x16, 30x16
    int Rows, Cols, squareSize, disFromTop, numNotBombs, trigerredX, trigerredY;
    void reset() {
        // memset(cover,0,sizeof(cover));
        memset(isBomb,0,sizeof(isBomb));
        memset(flagged,0,sizeof(flagged));
        for (int i=0;i<Rows;i++) for (int j=0;j<Cols;j++) cover[i][j] = true;
        int numBombs = Rows*Cols/10;
        // int numBombs = 1;
        int notBombs = Rows*Cols - numBombs;
        numNotBombs = notBombs;
        for (int i=0;i<Rows;i++) {
            for (int j=0;j<Cols;j++) {
                int x = rand() % (numBombs+notBombs);
                if (x<numBombs) {
                    numBombs--;
                    isBomb[i][j] = true;
                } else notBombs--;
            }
        }
        trigerredX = trigerredY = -1;
    }
    void unReset() {
    	for (int i=0;i<Rows;i++) for (int j=0;j<Cols;j++) if (!flagged[i][j]) cover[i][j] = false;
    }
    int countBombs(int x,int y) {
        int Haachamachama = 0;
        for (int i=0;i<8;i++) {
            if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) {
                Haachamachama += isBomb[x+xi[i]][y+yi[i]] ;
            }
        }
        return Haachamachama ;
    }
    int countFlags(int x,int y) {
        int Haachamachama = 0;
        for (int i=0;i<8;i++) {
            if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) {
                Haachamachama += flagged[x+xi[i]][y+yi[i]] ;
            }
        }
        return Haachamachama ;
    }
    void floodField(int x,int y) {
        if (!cover[x][y]) return;
        cover[x][y] = false;
        numNotBombs--;
        // cout << "floodField - ed\n" ;
        // cout << "part 2\n" ;
        if (countBombs(x,y)==0) {
            for (int i=0;i<8;i++) {
                if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) floodField(x+xi[i],y+yi[i]);
            }
        }
        // else {
        //     for (int i=0;i<8;i++) {
        //         if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) {
        //             if (!flagged[x+xi[i]][y+yi[i]]) cover[x+xi[i]][y+yi[i]] = false;
        //         }
        //     }
        // }
    }
    bool floodField2(int x,int y) {
        for (int i=0;i<8;i++) {
            if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols && !flagged[x+xi[i]][y+yi[i]]) {
                if (!isBomb[x+xi[i]][y+yi[i]]) floodField(x+xi[i],y+yi[i]);
                else {
                    trigerredX = x+xi[i];
                    trigerredY = y+yi[i];
                    return false;
                }
            }
        }
        return true;
    }
    void drawBoard(SDL_Renderer* renderer);
};