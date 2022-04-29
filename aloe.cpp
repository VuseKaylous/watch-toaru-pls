#include<iostream>
#include<cstring>
#include<SDL.h>
#include"aloe.h"

using namespace std;


bool isIn(int x1,int y1,int x2,int y2) {
    int x,y;
    SDL_GetMouseState(&x,&y);
    if (x1<x && x<x2 && y1<y && y<y2) return true;
    return false;
}

bool isInSDLRect(SDL_Rect rect) {
    return isIn(rect.x, rect.y, rect.x+rect.w, rect.y + rect.h);
}

string toString(int x) {
    if (x==0) return "0";
    string s;
    while (x>0) {
        char c = (x%10)+'0';
        s = c + s;
        x/=10;
    }
    return s;
}