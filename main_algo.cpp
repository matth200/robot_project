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

#include "src/m_learning.h"

//world
#include "src/world.h"

//draw
#include "src/draw.h"

//car
#include "src/car.h"

#include "src/utils.h"
#include "src/genetic_algorithm_const.h"

#include "src/display.h"

//include fs
#include <filesystem>
#include <regex>
namespace fs = std::filesystem;

//include time
#include <ctime>

using namespace std;
typedef chrono::high_resolution_clock::time_point time_point;

//parametre d'affichage
#define MAX_TIME_ESC 500
#define FPS 40.0

//parametre GENETIC_ALGORITHM
#include "src/genetic_algorithm_const.h"

//utils
#include "src/utils.h"



int evaluateRobotArduino(RobotArduino &robot, bool &f);


int main(int argc, char **argv){

	srand(time(NULL));

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cout << "Erreur de lancement de la sdl: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if(screen == NULL){
		cout << "Erreur d'initialisation d'une fenetre : " << SDL_GetError() << endl;
		return 1;
	}

	SDL_WM_SetCaption("ALGO SIMULATION", NULL);
	

	//on construit l'univers
	Universe universe(SCREEN_WIDTH,SCREEN_HEIGHT);
	cout << "Ouverture des maps..." << endl;
	universe.addLevel("../resources/map/map_1.level");
	universe.addLevel("../resources/map/map_2.level");
	universe.addLevel("../resources/map/map_3.level");
	universe.addLevel("../resources/map/map_4.level");
	universe.addLevel("../resources/map/map_5.level");
	universe.addLevel("../resources/map/map_6.level");
	// universe.addLevel("../resources/map/map.level");
	//on se mets premier niveau et première position
	universe.initStep();
	//construction des mondes virtuels pour la détection
	universe.buildUniverse();
	
	//Robot
	RobotArduino robot;
	robot.connectToUniverse(&universe);
	robotInit(robot);


	// //Display information
	Display display(SCREEN_WIDTH, 0, OUTSCREEN_W, SCREEN_HEIGHT);
	display.setRobot((Robot*)&robot);
	display.setUniverse(&universe);


	//speed
	bool state_space = true, state_enter = false;
	bool behind_work = false;
	bool state_save = false;

    int score = 0;
	int max_score = 0;

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
						case SDLK_SPACE:
							state_space = true;
							break;
						case SDLK_RIGHT:
							//robot.setMotor2(ROBOT_SPEED);
							break;
						case SDLK_LEFT:
							//robot.setMotor1(ROBOT_SPEED);
							break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym){
						case SDLK_SPACE:
							state_space = false;
							break;
						case 13:
							state_enter = false;
							break;
						case SDLK_RIGHT:
							//robot.setMotor2(0);
							break;
						case SDLK_LEFT:
							//robot.setMotor1(0);
							break;
					}
					break;
			}
		}
		//affichage
		SDL_FillRect(screen, NULL, COLOR_BLACK);

		bool finish = false;


		score += evaluateRobotArduino(robot, finish);

		//on gère les infos
		if(max_score<score){
			max_score = score;
		}
		display.setInfo(0, max_score, score);

        //affichage de l'environement
        universe.getCurrentWorld()->draw(screen);
        robot.draw(screen);
		display.draw(screen);


		//on affiche
		SDL_Flip(screen);

		if(finish){
			//si pas fini on passe au niveau d'au dessus
			universe.nextStep();
			if(!universe.isFinished()){
				robotInit(robot);
				this_thread::sleep_for(chrono::milliseconds(200));
			}
		}

		//management time
		end_point = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(end_point-start_point).count();
		if(duration<1000.0/FPS&&state_space){
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(1000.0/FPS-duration)));
		}
	}
    return 0;
}


int evaluateRobotArduino(RobotArduino &robot, bool &f){
	//si trop long, on arrete
	robot.loop();
	int score = 0;

	if(robot.getDuration()>90000){
		robot.setAlive(false);
	}
	f = false;
	//si il est mort ou à gagner, on gére la sélection
	if(!robot.isAlive()||robot.getWin()){
		//si il a reussi, il doit essayer d'avoir le chemin le plus court
		if(robot.getWin()){
			score += int(50000.0-robot.getDistanceDone());
		//sinon le plus long
		}else{
			score += int(robot.getDistanceDone());
		}
		f = true;
	}
	return score;
}