#include <fstream>
#include <iostream>
#include <cmath>

//random librairy
#include <cstdlib>
#include <ctime>

//management time
#include <chrono>
#include <thread>

//SDL
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <vector>

//machine learning
#include "src/m_learning.h"

using namespace std;
typedef chrono::high_resolution_clock::time_point time_point;


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define OUTSCREEN_W 500
#define MAX_TIME_ESC 500
#define COLOR_RED SDL_MapRGB(screen->format, 245,10,10)
#define COLOR_WHITE SDL_MapRGB(screen->format, 255,255,255)
#define COLOR_BLACK SDL_MapRGB(screen->format, 0,0,0)

#define RANDOM_VALUE_W 10
#define RANDOM_VALUE_B 10

#define FPS 40.0

struct Line{
    int x1,y1;
    int x2,y2;
};

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawLine(SDL_Surface *screen, Line line, Uint32 color);
void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m);
void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color);

int main(int argc, char **argv){

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cout << "Erreur de lancement de la sdl: " << SDL_GetError() << endl;
		return 1;
	}

	if(TTF_Init() < 0){
		cout << "Erreur de lancement de sdl_ttf: " << TTF_GetError() << endl;
		return 1;
	}

	atexit(TTF_Quit);
	atexit(SDL_Quit);

	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W)

	SDL_WM_SetCaption("NETWORK TRAINING", NULL);


    return 0;
}

void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m)
{
	int size = 24;
	for(int j(0);j<m.getNumberColumn();j++)
	{
		for(int i(0);i<m.getNetwork(j)->get_number_neuron();i++)
		{
			//on affiche chaque neurone avec sa valeur plus ou moins blanche
			double value = m.getNetwork(j)->get_neuron(i)->get_value();
			Uint32 color = SDL_MapRGB(screen->format,value*255.0,50+value*205.0,value*255.0);

			//si c'est la direction souhaité
			if(m.getNumberColumn()-1==j&&i==m.getPrediction())
				color = SDL_MapRGB(screen->format,25,200,200);

			drawSquare(screen,SCREEN_WIDTH+40+j*100,50+((size-m.getNetwork(j)->get_number_neuron())/2.0+i)*30,20,20,color);
			for(int a(0);a<m.getNetwork(j)->get_neuron(i)->numberConnection()&&j!=0;a++)
			{
				value = m.getNetwork(j-1)->get_neuron(a)->get_value();
				color = SDL_MapRGB(screen->format,value*25,value*25,value*200);
				if(m.getNetwork(j)->get_neuron(i)->get_weight(a)<0)
					color = SDL_MapRGB(screen->format,value*200,value*25,value*25);
				drawLine(screen,SCREEN_WIDTH+40+(j-1)*100+20,50+((size-m.getNetwork(j-1)->get_number_neuron())/2.0+a)*30+10,SCREEN_WIDTH+40+j*100,50+((size-m.getNetwork(j)->get_number_neuron())/2.0+i)*30+10,color);
			}
		}
	}
}

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2){
    ofstream file(filename, ios::binary);
    if(!file.is_open()){
        return true;
    }

    int cursor = 0;
    int size = liste1.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<liste1.size();i++){
        Line line(liste1[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    }

    size = liste2.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<liste2.size();i++){
        Line line(liste2[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    }


    return false;
}

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color){
    *((Uint32*)(screen->pixels) + x + y * screen->w) = color;
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