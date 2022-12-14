#include "draw.h"
using namespace std;

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color){
    if(screen->w>x&&x>=0&&screen->h>y&&y>=0){
        *((Uint32*)(screen->pixels) + x + y * screen->w) = color;
    }
}


void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color)
{
	for(int iw(0);iw<w;iw++)
	{
		for(int ih(0);ih<h;ih++)
		{
			setPixel(screen,x+iw,y+ih,color);
		}
	}
}


void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color){
    int deltaX = x2-x1;
    int deltaY = y2-y1;
    int h = (int)sqrt(deltaX*deltaX + deltaY*deltaY) + 1;
    if(h!=0){
        double rapportX = (double(deltaX)/h),
               rapportY = (double(deltaY)/h);
        for(int i(0);i<h;i++){
            int x = int(x1+rapportX*i),
                y =  int(y1+rapportY*i);
            setPixel(screen, x,y, color);
        }
    }
}

void drawLine(SDL_Surface *screen, Line line, Uint32 color){
    drawLine(screen, line.x1, line.y1, line.x2, line.y2, color);
}
void drawLine(SDL_Surface *screen, LineD line, Uint32 color){
    drawLine(screen, (int)line.x1, (int)line.y1, (int)line.x2, (int)line.y2, color);
}


void drawCircle(SDL_Surface *screen, int x, int y, int rayon, Uint32 color){
    const int resolution = 10;
    const double angle = 2*M_PI/resolution;
    setPixel(screen, x,y, color);
    for(int i=0;i<resolution;++i){
        drawLine(screen, x + rayon*cos(angle*i), y - rayon*sin(angle*i), x + rayon*cos(angle*(i+1)), y - rayon*sin(angle*(i+1)), color);
    }

}