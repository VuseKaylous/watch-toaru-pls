
#include "subScreen.h"

SUBSCREEN::SUBSCREEN() {
	boingboing.w = 150;
	boingboing.h = 100;
	boingboing.x = boingboing.y = 0;
}
SUBSCREEN::SUBSCREEN(int width, int height) {
	boingboing.w = width;
	boingboing.h = height;
	boingboing.x = boingboing.y = 0;
}
void SUBSCREEN::settingUpSubScreen(int width, int height) {
	mainScreen.w = width;
	mainScreen.h = height;
	mainScreen.x = mainScreen.y = 0;
}
void SUBSCREEN::setSubScreen(int mode) {
	if (mode == 0) boingboing = mainScreen;
	else {
		boingboing = {0,0,300,200};
	}
}
void SUBSCREEN::subScreen_event_handling(int x, int y) {
	// std::cout << x << " " << y << "\n" ;
	int xi = 0, yi = 0, speed = 5;
	if (x <= mainScreen.x+1) xi = -speed;
	if (y <= mainScreen.y+1) yi = -speed;
	if (x >= mainScreen.x-1 + mainScreen.w) xi = speed;
	if (y >= mainScreen.y-1 + mainScreen.h) yi = speed;
	if (boingboing.x + xi >= mainScreen.x &&
		boingboing.y + yi >= mainScreen.y &&
		boingboing.x + xi + boingboing.w <= mainScreen.x + mainScreen.w &&
		boingboing.y + yi + boingboing.h <= mainScreen.y + mainScreen.h )
	{
		boingboing.x += xi;
		boingboing.y += yi;
	}
}