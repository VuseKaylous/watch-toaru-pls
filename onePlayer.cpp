
#include <SDL.h>
#include "sora.h"
#include "aloe.h"
#include "rushia.h"
#include "onePlayer.h"

using namespace std;

ONEPLAYER::ONEPLAYER() {
    board.BOARDfree();
    
    chosenDifficulty = 1;
    onePlayerChosenBackgroundX = 0;
    chosenWalfie = 0;

    onePlayerChosenBackgroundY = rand() % onePlayerBackgroundSize;
    winningOpacity = 0;
    winningShowUp = 0;
}

ONEPLAYER::~ONEPLAYER() {
    ONEPLAYERfree();
}

string onePlayerBackground[] = {"0.jpg", "1.jpg", "2.jpg", "3.png"};

void ONEPLAYER::ONEPLAYERfree() {
    SDL_DestroyTexture(RestartButton);
    SDL_DestroyTexture(winning);
    for (int i=0;i<3;i++) {
        SDL_DestroyTexture(onePlayerDecoratingTextureGif[i]);
        onePlayerDecoratingTextureGif[i] = NULL;
    }
    RestartButton = winning = NULL;
    menu.free();
    for (int i=0;i<onePlayerBackgroundSize;i++) {
        SDL_DestroyTexture(onePlayerBackgroundTexture[0][i]);
        SDL_DestroyTexture(onePlayerBackgroundTexture[1][i]);
        onePlayerBackgroundTexture[0][i] = onePlayerBackgroundTexture[1][i] = NULL;
    }
    for (int i=0;i<39;i++) {
        SDL_DestroyTexture(onePlayerBackgroundTextureGif[i]);
        onePlayerBackgroundTextureGif[i] = NULL;
    }
}

bool ONEPLAYER::loadOnePlayer(SDL_Renderer *renderer, TTF_Font *gFont) {
    bool success = true, checkSuccess = true;

    checkSuccess = board.loadRushia(renderer);
    if (!checkSuccess) success = false;

    string loadingPictures = "picture/SDL_image_related/";

    loadingPictures = "picture/SDL_image_related/RestartButton.png";
    RestartButton = loadSurface(loadingPictures, renderer);
    if (RestartButton == NULL) success = false;
    // if (!success) logSDLError(cout, "RestartButton", true);
    
    loadingPictures = "picture/SDL_image_related/winning.png";
    winning = loadSurface(loadingPictures, renderer);
    if (winning == NULL) success = false;
    // if (!success) logSDLError(cout, "winning", true);
    
    // loadingPictures = "picture/SDL_image_related/menu.png";
    // menu = loadSurface(loadingPictures, renderer);
    // if (menu == NULL) success = false;
    // if (!success) logSDLError(cout, "menu", true);

    SDL_Color textColor = {0, 0, 0};
    if (!menu.loadFromRenderedText("MENU", textColor, renderer, gFont)) success = false;

    loadingPictures = "picture/SDL_image_related/walfie/out-transparent-";
    for (int i=0;i<3;i++) {
        string si = loadingPictures + toString(i) + ".png";
        onePlayerDecoratingTextureGif[i] = loadSurface(si, renderer);
        if (onePlayerDecoratingTextureGif[i] == NULL) success = false;
    }
    
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
    
    
    return success;
}

void ONEPLAYER::restart1p() {
    board.setDifficulty(chosenDifficulty);
    board.squareSize = playField.w/board.Cols;
    board.reset();

    onePlayerChosenBackgroundY = rand() % onePlayerBackgroundSize;

    winningOpacity = 0;
    winningShowUp = 0;

    // startTime = time(0);

    // cout << board.numNotBombs << "\n" ;
}

void ONEPLAYER::settingUp1p(int screen_width, int screen_height) {
    miniScreen.mainScreen = {0,0,screen_width,screen_height};
    miniScreen.boingboing = miniScreen.mainScreen;

    board.squareSize = screen_width/board.Cols;
    playField = {0,screen_height - board.Rows * board.squareSize,screen_width, board.Rows * board.squareSize};

    // OnePlayer.RestartRect = {(SCREEN_WIDTH-board.squareSize)/2,(OnePlayer.playField.y-board.squareSize)/2,board.squareSize*2,board.squareSize*2};
    RestartRect.w = RestartRect.h = playField.y/2;
    RestartRect.x = screen_width/2 - RestartRect.w/2;
    RestartRect.y = RestartRect.h/2;

    MenuRect = {playField.y/4, playField.y/4, playField.y, playField.y/2};
    MenuRect.w = MenuRect.h / menu.height * menu.width + menu.height;
    // cout << OnePlayer.MenuRect.x << " " << OnePlayer.MenuRect.y << " " << OnePlayer.MenuRect.w << " " << OnePlayer.MenuRect.h << "\n" ;
    walfieRect = {screen_width - playField.y/2*3, 0, playField.y/2*3 , playField.y};
    walfieMovingRect = walfieRect;
}

//------------------------------------ one-player-related --------------------------------------

void ONEPLAYER::board_event_handling(SDL_Event e) {
    int x,y;
    // mouse.getExactPos(miniScreen);
    x = mouse.posX;
    y = mouse.posY;
    if (e.type == SDL_MOUSEBUTTONUP) {
        
        // SDL_GetMouseState(&x,&y);
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

void ONEPLAYER::onePlayer_event_handling(SDL_Event e, int &current_state) {
    // mouse.getExactPos(miniScreen);
    if (isInSDLRect(playField, mouse.posX, mouse.posY)) {
        board_event_handling(e);
    }
    else if (isInSDLRect(RestartRect, mouse.posX, mouse.posY)) {
        if (e.type == SDL_MOUSEBUTTONUP) restart1p();
    }
    else if (isInSDLRect(MenuRect, mouse.posX, mouse.posY)) {
        if (e.type == SDL_MOUSEBUTTONUP) {
            current_state = 0; // menuScreen
            // SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
            // SDL_RenderClear(renderer);
        }
    }
}

void ONEPLAYER::drawWalfie(SDL_Renderer *renderer) {
    chosenWalfie = (chosenWalfie + 1)%12;
    SDL_Rect fillRect = walfieMovingRect;
    bool fuck = true;
    while (fillRect.x + fillRect.w > 0) {
        if (fuck) SDL_RenderCopy(renderer, onePlayerDecoratingTextureGif[chosenWalfie/4], NULL, &fillRect);
        fillRect.x -= fillRect.w;
        fuck = 1 - fuck;
    }
}

void ONEPLAYER::drawOnePlayer(SDL_Renderer *renderer, bool MouseDown) {
    if (onePlayerChosenBackgroundX == 2) {
        SDL_RenderCopy(renderer, onePlayerBackgroundTextureGif[onePlayerChosenBackgroundY/4], NULL, &playField);
        onePlayerChosenBackgroundY = (onePlayerChosenBackgroundY + 1)%(39*4);
    } else {
        if (onePlayerChosenBackgroundY >= onePlayerBackgroundSize) onePlayerChosenBackgroundY = rand() % onePlayerBackgroundSize;
        SDL_RenderCopy(renderer, onePlayerBackgroundTexture[onePlayerChosenBackgroundX][onePlayerChosenBackgroundY], NULL, &playField);
    }
    board.drawBoard(renderer, playField, MouseDown, mouse.posX, mouse.posY);

    walfieMovingRect.x --;
    if (walfieMovingRect.x < walfieRect.x - walfieRect.w) walfieMovingRect.x = walfieRect.x + walfieRect.w;
    drawWalfie(renderer);

    SDL_RenderCopy(renderer, RestartButton, NULL, &RestartRect);
    // SDL_RenderCopy(renderer, menu, NULL, &MenuRect);
    
    SDL_SetRenderDrawColor(renderer,255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &MenuRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &MenuRect);
    menu.render(MenuRect, renderer);

    if (board.numNotBombs <= 0 && winningShowUp == 0) winningShowUp = 5;

    if (winningShowUp!=0) {
        if (winningShowUp > 0 && winningOpacity < 255) winningOpacity += winningShowUp;
        if (winningShowUp < 0 && winningOpacity > 0) winningOpacity += winningShowUp;
        SDL_SetTextureAlphaMod(winning, winningOpacity);
        SDL_RenderCopy(renderer, winning, NULL, &playField);
    }
}