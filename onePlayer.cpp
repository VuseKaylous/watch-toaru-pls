
#include <SDL.h>
#include "sora.h"
#include "aloe.h"
#include "rushia.h"
#include "onePlayer.h"

using namespace std;

ONEPLAYER::ONEPLAYER() {
    chosenDifficulty = 1;
    onePlayerChosenBackgroundX = 0;
}

string onePlayerBackground[] = {"0.jpg", "1.jpg", "2.jpg", "3.png"};

bool ONEPLAYER::loadOnePlayer(SDL_Renderer *renderer) {
    bool success = true;
    string loadingPictures = "picture/SDL_image_related/";
    /*
    for (int i=0;i<picCharSize;i++) {
        string si;
        if (i<picCharSize-1) si = loadingPictures + picChar[i] + ".png";
        else si = loadingPictures + picChar[i] ;
        pic[i] = loadSurface(si,renderer);
        if (pic[i] == NULL) {
            cout << "Failed to load image " << picChar[i] << "\n" ;
            success = false;
            break;
        }
    }
    */
    loadingPictures = "picture/SDL_image_related/RestartButton.png";
    RestartButton = loadSurface(loadingPictures, renderer);
    if (RestartButton == NULL) success = false;
    // if (!success) logSDLError(cout, "RestartButton", true);
    
    loadingPictures = "picture/SDL_image_related/winning.png";
    winning = loadSurface(loadingPictures, renderer);
    if (winning == NULL) success = false;
    // if (!success) logSDLError(cout, "winning", true);
    
    loadingPictures = "picture/SDL_image_related/menu.png";
    menu = loadSurface(loadingPictures, renderer);
    if (menu == NULL) success = false;
    // if (!success) logSDLError(cout, "menu", true);
    
    /*
    loadingPictures = "picture/background_material/AH/";
    for (int i=0;i<onePlayerBackgroundSize;i++) {
        string si;
        si = loadingPictures + onePlayerBackground[i] ;
        onePlayerBackgroundTexture[0][i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTexture[0][i] == NULL) {
            cout << "Failed to load image AH" << onePlayerBackground[i] << "\n" ;
            success = false;
            break;
        }
    }
    if (!success) logSDLError(cout, "AH", true);
    loadingPictures = "picture/background_material/SS/";
    for (int i=0;i<onePlayerBackgroundSize;i++) {
        string si;
        si = loadingPictures + onePlayerBackground[i] ;
        onePlayerBackgroundTexture[1][i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTexture[1][i] == NULL) {
            cout << "Failed to load image SS" << onePlayerBackground[i] << "\n" ;
            success = false;
            break;
        }
    }
    if (!success) logSDLError(cout, "SS", true);
    loadingPictures = "picture/background_material/SS_gif/gifs/";
    for (int i=0;i<39;i++) {
        string si;
        si = loadingPictures + toString(i) + ".gif" ;
        onePlayerBackgroundTextureGif[i] = loadSurface(si,renderer);
        if (onePlayerBackgroundTextureGif[i] == NULL) {
            cout << "Failed to load image background gif" << i << "\n" ;
            success = false;
            break;
        }
    }
    if (!success) logSDLError(cout, "SS_gif", true);
    */
    
    return success;
}

void ONEPLAYER::restart1p(BOARD &board) {
    board.setDifficulty(chosenDifficulty);
    board.reset();

    onePlayerChosenBackgroundY = rand() % onePlayerBackgroundSize;

    winningOpacity = 0;
    winningShowUp = 0;

    // startTime = time(0);

    // cout << board.numNotBombs << "\n" ;
}

//------------------------------------ one-player-related --------------------------------------

void ONEPLAYER::board_event_handling(BOARD &board, SDL_Event e) {
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

void ONEPLAYER::drawOnePlayer(BOARD &board, SDL_Renderer *renderer, bool MouseDown) {
    // SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY/2], NULL, &playField);
    if (onePlayerChosenBackgroundX == 2) {
        // SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY/4], NULL, &playField);
        onePlayerChosenBackgroundY = (onePlayerChosenBackgroundY + 1)%(39*4);
    } else {
        // SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY], NULL, &playField);
    }
    board.drawBoard(renderer, playField, MouseDown);
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