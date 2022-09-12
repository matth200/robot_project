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

//affichage information
#include "src/display.h"

using namespace std;
typedef chrono::high_resolution_clock::time_point time_point;


#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define OUTSCREEN_W 400
#define MAX_TIME_ESC 500

#define RANDOM_VALUE_W 100
#define RANDOM_VALUE_B 100

#define FPS 40.0

//variable pour effectuer la selection
struct VarSelection
{
	MachineLearning m;
	int score;
	bool best;
};

//genetic algorithm
void getAdn(MachineLearning &m, vector<unsigned int> &adn);
void setAdn(MachineLearning &m, vector<unsigned int> &adn);
void makeBabys(MachineLearning &m1, MachineLearning &m2);
VarSelection selectionRandomly(vector<VarSelection> &players, int &a);
double distance(int x1, int y1, int x2, int y2);

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

	TTF_Font *police = TTF_OpenFont("../resources/fonts/pixel_font.ttf",16);

	atexit(TTF_Quit);
	atexit(SDL_Quit);

	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if(screen == NULL){
		cout << "Erreur d'initialisation d'une fenetre : " << SDL_GetError() << endl;
		return 1;
	}

	SDL_WM_SetCaption("NETWORK TRAINING", NULL);


	World world(SCREEN_WIDTH,SCREEN_HEIGHT);
	if(!world.loadMap("../resources/map/map.level")){
		cout << "Erreur de chargement de la map" << endl;
	}
	world.buildVirtualWorld();

	//Machine learning
	MachineLearning machine(2);
	machine.addColumn(10);
	machine.addColumn(2);
	machine.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);

	//machine.saveTraining("../resources/trained_model/brain.ml");
	
	//Robot
	Robot robot;
	robot.setPos(500.0,500.0);
	robot.setRotation(M_PI/3.0);
	robot.setBrain(&machine);
	robot.connectToWorld(world);

	//Display information
	Display display(SCREEN_WIDTH, 0, OUTSCREEN_W, SCREEN_HEIGHT);
	display.setNeuralNetwork(&machine);


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
							//robot.setMotor2(ROBOT_SPEED);
							break;
						case SDLK_LEFT:
							//robot.setMotor1(ROBOT_SPEED);
							break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym){
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

		//affichage de l'environement
		display.draw(screen);
		world.draw(screen);
		robot.update();
		if(!robot.isAlive()){
			continuer = false;
		}
		robot.draw(screen);

		//on affiche
		SDL_Flip(screen);
		//management time
		end_point = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(end_point-start_point).count();
		if(duration<1000.0/FPS){
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(1000.0/FPS-duration)));
		}
	}

	TTF_CloseFont(police);
    return 0;
}



void makeBabys(MachineLearning &m1, MachineLearning &m2)
{
	//get the adn 
	vector<unsigned int> adn1, adn2, adnT1, adnT2;
	//pour qu'il prenne les meme réseaux de neurone
	getAdn(m1,adn1);
	getAdn(m2,adn2);
	ofstream log("../log/logBabys.txt");

	//mix the adn <<<----
	log << "adn |";
	int cursor = 1+rand()%(adn1.size()-2);
	for(int i(0);i<adn1.size();i++)
	{
		if(i<cursor)
		{
			log << "O";
			adnT1.push_back(adn1[i]);
			adnT2.push_back(adn2[i]);
		}else{
			log << "M";
			adnT1.push_back(adn2[i]);
			adnT2.push_back(adn1[i]);
		}
	}

	//set the adn
	setAdn(m1,adnT1);
	setAdn(m2,adnT2);
}

void getAdn(MachineLearning &m, vector<unsigned int> &adn)
{
	adn.clear();
	for(int l(0);l<m.getNumberColumn()-1;l++)
	{
		for(int j(0);j<m.getNetwork(l+1)->get_number_neuron();j++)
		{
			for(int i(0);i<m.getNetwork(l+1)->get_neuron(j)->numberConnection();i++)
			{
				adn.push_back((m.getNetwork(l+1)->get_neuron(j)->get_weight(i)+RANDOM_VALUE_W/2.0)/RANDOM_VALUE_W*4294967296);
			}
			adn.push_back((m.getNetwork(l+1)->get_neuron(j)->get_bias()+RANDOM_VALUE_B/2.0)/RANDOM_VALUE_B*4294967296);
		}
	}
}

void setAdn(MachineLearning &m, vector<unsigned int> &adn)
{
	int index = 0;
	for(int l(0);l<m.getNumberColumn()-1;l++)
	{
		for(int j(0);j<m.getNetwork(l+1)->get_number_neuron();j++)
		{
			for(int i(0);i<m.getNetwork(l+1)->get_neuron(j)->numberConnection();i++)
			{
				//m.getNetwork(l+1)->get_neuron(j)->set_weight(i,adn[index]);
				index++;
			}
			//m.getNetwork(l+1)->get_neuron(j)->set_bias(adn[index]);
			index++;
		}
	}

	if(adn.size()==index)
	{
		index = 0;
		for(int l(0);l<m.getNumberColumn()-1;l++)
		{
			for(int j(0);j<m.getNetwork(l+1)->get_number_neuron();j++)
			{
				for(int i(0);i<m.getNetwork(l+1)->get_neuron(j)->numberConnection();i++)
				{
					m.getNetwork(l+1)->get_neuron(j)->set_weight(i,adn[index]/4294967296.0*RANDOM_VALUE_W-RANDOM_VALUE_W/2.0);
					index++;
				}
				m.getNetwork(l+1)->get_neuron(j)->set_bias(adn[index]/4294967296.0*RANDOM_VALUE_B-RANDOM_VALUE_B/2.0);
				index++;
			}
		}
	}
	else{
		ofstream log("errorSetADN.txt");
		log << "error" << endl;
	}
}

VarSelection selectionRandomly(vector<VarSelection> &players, int &a)
{
	int sum = 0;
	
	//selection with the index 
	for(int i(0);i<players.size();i++)
	{
		sum+=pow(players.size()-(i+1),2);
	}

	int arrow = rand()%sum;
	int valeurCum = 0;
	bool done = 0;
	for(int i(0);i<players.size()&&!done;i++)
	{
		valeurCum+=pow(players.size()-(i+1),2);
		if(valeurCum>arrow)
		{		
			a = i;
			return players[i];
		}
	}
	return players[0];
}

double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}
