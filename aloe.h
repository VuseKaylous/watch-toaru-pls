#ifndef ALOE_H
#define ALOE_H
#include<iostream>
#include<cstring>
#include<SDL.h>

using namespace std;

// bool isIn(int x1,int y1,int x2,int y2);

bool isInSDLRect(SDL_Rect rect, int x, int y);

string toString(int x);

#endif