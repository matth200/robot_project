#ifndef INCLUDE_DRAW
#define INCLUDE_DRAW

#include <SDL/SDL.h>
#include <cmath>


#define COLOR_RED SDL_MapRGB(screen->format, 245,10,10)
#define COLOR_WHITE SDL_MapRGB(screen->format, 255,255,255)
#define COLOR_BLACK SDL_MapRGB(screen->format, 0,0,0)
#define COLOR_GREEN SDL_MapRGB(screen->format, 10,220,10)

struct Line{
    int x1,y1;
    int x2,y2;
};
struct LineD{
    double x1, y1;
    double x2, y2;
};


void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawLine(SDL_Surface *screen, Line line, Uint32 color);
void drawLine(SDL_Surface *screen, LineD line, Uint32 color);
void drawCircle(SDL_Surface *screen, int x, int y, int rayon,  Uint32 color);

#endif