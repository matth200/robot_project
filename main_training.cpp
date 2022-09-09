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

//world
#include "src/world.h"
#include "src/draw.h"


//car
#include "src/car.h"

using namespace std;
typedef chrono::high_resolution_clock::time_point time_point;


#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define OUTSCREEN_W 300
#define MAX_TIME_ESC 500

#define RANDOM_VALUE_W 10
#define RANDOM_VALUE_B 10

#define ROBOT_SPEED 1

#define FPS 40.0

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2);
void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m);

int main(int argc, char **argv){

	srand(time(NULL));

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
	screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if(screen == NULL){
		cout << "Erreur d'initialisation d'une fenetre : " << SDL_GetError() << endl;
		return 1;
	}

	SDL_WM_SetCaption("NETWORK TRAINING", NULL);


	World world;
	if(!world.loadMap("../resources/map/map.level")){
		cout << "Erreur de chargement de la map" << endl;
	}

	//Machine learning
	MachineLearning machine(2);
	machine.addColumn(10);
	machine.addColumn(2);
	machine.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);

	machine.calcul();


	//Robot
	Car robot;
	robot.setPos(500,500);
	robot.setRotation(M_PI/3.0);

	//boucle
	bool continuer = true;
	time_point start_point, end_point;
	SDL_Event event;
	double duration;
	while(continuer){
		start_point = chrono::high_resolution_clock::now();
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					continuer = false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_ESCAPE:
							continuer = false;
							break;
						case SDLK_RIGHT:
							robot.setMotor2(ROBOT_SPEED);
							break;
						case SDLK_LEFT:
							robot.setMotor1(ROBOT_SPEED);
							break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym){
						case SDLK_RIGHT:
							robot.setMotor2(0);
							break;
						case SDLK_LEFT:
							robot.setMotor1(0);
							break;
					}
					break;
			}
		}
		//affichage
		SDL_FillRect(screen, NULL, COLOR_BLACK);

		world.draw(screen);
		robot.forward();
		robot.draw(screen);

		drawNeuralNetwork(screen, machine);

		SDL_Flip(screen);
		//management time
		end_point = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(end_point-start_point).count();
		if(duration<1000.0/FPS){
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(1000.0/FPS-duration)));
		}
	}
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
				//color = SDL_MapRGB(screen->format,200,25,25);
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