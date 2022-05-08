#include<cstring>
#include<SDL.h>
#include "sora.h"
#include "aloe.h"
#include"rushia.h"

using namespace std;

void BOARD::BOARDfree() {
    SDL_DestroyTexture(bomb);
    SDL_DestroyTexture(flag);
    SDL_DestroyTexture(trigerredBomb);
    bomb = flag = trigerredBomb = NULL;
    for (int i=0;i<=9;i++) {
        SDL_DestroyTexture(numbers[i]);
        numbers[i] = NULL;
    }
}

bool BOARD::loadRushia(SDL_Renderer *renderer) {
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
    loadingPictures = "picture/SDL_image_related/bomb.png";
    bomb = loadSurface(loadingPictures, renderer);
    if (bomb == NULL) success = false;

    loadingPictures = "picture/SDL_image_related/flag.png";
    flag = loadSurface(loadingPictures, renderer);
    if (flag == NULL) success = false;
    
    loadingPictures = "picture/SDL_image_related/trigerredBomb.png";
    trigerredBomb = loadSurface(loadingPictures, renderer);
    if (trigerredBomb == NULL) success = false;
    
    return success;
}

void BOARD::setDifficulty(int chosenDifficulty) {
    switch (chosenDifficulty) {
        case 0:
            Rows = 18;
            Cols = 32;
            difficulty = 8;
            break;
        case 1:
            Rows = 27;
            Cols = 48;
            difficulty = 8;
            break;
        case 2:
            Rows = 27;
            Cols = 48;
            difficulty = 5;
            break;
    }
}

BOARD::BOARD() {
    setDifficulty(1);
}

void BOARD::reset() {
    // memset(cover,0,sizeof(cover));
    memset(isBomb,0,sizeof(isBomb));
    memset(flagged,0,sizeof(flagged));
    for (int i=0;i<Rows;i++) for (int j=0;j<Cols;j++) cover[i][j] = true;
    int numBombs = Rows*Cols/difficulty;
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
void BOARD::unReset() {
	for (int i=0;i<Rows;i++) for (int j=0;j<Cols;j++) if (!flagged[i][j]) cover[i][j] = false;
}
int BOARD::countBombs(int x,int y) {
    int Haachamachama = 0;
    for (int i=0;i<8;i++) {
        if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) {
            Haachamachama += isBomb[x+xi[i]][y+yi[i]] ;
        }
    }
    return Haachamachama ;
}
int BOARD::countFlags(int x,int y) {
    int Haachamachama = 0;
    for (int i=0;i<8;i++) {
        if (0<=x+xi[i] && x+xi[i]<Rows && 0<=y+yi[i] && y+yi[i]<Cols) {
            Haachamachama += flagged[x+xi[i]][y+yi[i]] ;
        }
    }
    return Haachamachama ;
}
void BOARD::floodField(int x,int y) {
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
bool BOARD::floodField2(int x,int y) {
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
void BOARD::drawBoard(SDL_Renderer* renderer, SDL_Rect playField, bool MouseDown) {
    // SDL_SetRenderDrawColor(renderer, 0,0,0,20);
    // SDL_RenderFillRect(renderer, &playField);
    for (int i=0;i<Rows;i++) {
        for (int j=0;j<Cols;j++) {
            // drawSquare(0+j*squareSize, SCREEN_HEIGHT - (Rows-i)*squareSize, squareSize,squareSize,cover[i][j], renderer, countBombs(i,j), i,j);
            drawSquare(playField.x + j*squareSize, playField.y + i*squareSize, squareSize,squareSize, renderer, i, j, MouseDown);
            // cout << countBombs(i,j) << "\n" ;
        }
    }
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    for (int i=0;i<=Rows;i++) {
        SDL_RenderDrawLine(renderer, playField.x, playField.y + i*squareSize, playField.x + playField.w, playField.y + i*squareSize);
    }
    for (int i=0;i<=Cols;i++) {
        SDL_RenderDrawLine(renderer, i*squareSize, playField.y, i*squareSize, playField.y + playField.h );
    }
    
}

void BOARD::drawSquare(int x,int y, int w,int h, SDL_Renderer* renderer, int xi,int yi, bool MouseDown) {
    SDL_Rect fillRect = { x, y, w, h };
    if (cover[xi][yi]) {
        if (isIn(x,y,x+w,y+h)) { // hovering
            if (MouseDown) {
                SDL_SetRenderDrawColor(renderer, 117, 202, 255, 200);
                SDL_RenderFillRect(renderer, &fillRect);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 28, 149, 201, 120);
                // SDL_SetTextureAlphaMod(winning, winningOpacity);
                SDL_RenderFillRect( renderer, &fillRect );
            }
            
        }
        // else SDL_SetRenderDrawColor( renderer, 116, 150, 168, 255 );
        
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