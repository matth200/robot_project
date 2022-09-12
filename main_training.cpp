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


//parametre d'affichage
#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define OUTSCREEN_W 400
#define MAX_TIME_ESC 500
#define FPS 40.0

//parametre GENETIC_ALGORITHM
#define NBR_POPULATION 100
#define FRQ_MUTATION 0.08
#define NBR_SELECTION 50
#define NBR_RANDOM 10
#define TIMEOUT 20000

//parametre machine learning
#define RANDOM_VALUE_W 10
#define RANDOM_VALUE_B 10

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

void evaluateRobot(Robot &r, VarSelection *player, bool &f);
void robotInit(Robot &robot);

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
	TTF_Font *bigFont = TTF_OpenFont("../resources/fonts/pixel_font.ttf",200);

	atexit(TTF_Quit);
	atexit(SDL_Quit);

	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if(screen == NULL){
		cout << "Erreur d'initialisation d'une fenetre : " << SDL_GetError() << endl;
		return 1;
	}

	SDL_WM_SetCaption("NEURAL NETWORK TRAINING", NULL);


	World world(SCREEN_WIDTH,SCREEN_HEIGHT);
	if(!world.loadMap("../resources/map/map.level")){
		cout << "Erreur de chargement de la map" << endl;
	}
	//monde virtuel pour la détection
	world.buildVirtualWorld();


	//on prepare le genetic algorithm
	vector<VarSelection> listeBrains;
	//on commence avec toutes la selection aux hasards
	for(int i(0);i<NBR_POPULATION;i++){
		VarSelection selection;
		MachineLearning *m = &(selection.m);
		m->open(4);
		m->addColumn(10);
		m->addColumn(10);
		m->addColumn(4);

		selection.best = false;
		selection.score = 0;

		m->setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
		listeBrains.push_back(selection);
	}

	//Machine learning, on fait jouer le joueur 1
	VarSelection *player = &(listeBrains[0]);
	int generation = 0;
	
	//Robot
	Robot robot;
	robotInit(robot);
	robot.setBrain(&(player->m));
	robot.connectToWorld(world);

	//Display information
	Display display(SCREEN_WIDTH, 0, OUTSCREEN_W, SCREEN_HEIGHT);
	display.setNeuralNetwork(&(player->m));
	display.setFont(police);
	display.setBigFont(bigFont);
	display.setRobot(&robot);


	//speed
	bool state_space = false;

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

		//viens faire avancer le robot et remplir si terminé le score du player
		bool finish;
		evaluateRobot(robot, player, finish);

		display.setInfo(generation, player->score);

		//affichage de l'environement
		world.draw(screen);
		robot.draw(screen);
		display.draw(screen);


		//on affiche
		SDL_Flip(screen);

		if(finish){
			//on determine les scores de chaque brain
			cout << "affichage non classé" << endl;

			cout << 0 << ", score:" << player->score << endl;
			for(int i(1);i<listeBrains.size();i++){
				player = &(listeBrains[i]);
				robotInit(robot);
				robot.setBrain(&(player->m));
				finish = false;
				//on le fait jouer tout seul
				while(!finish){
					evaluateRobot(robot, player, finish);
				}
				cout << i << ", score:" << player->score << endl;
			}
			//on les classe par rapport à leur score
			vector<VarSelection> listeBrainsTmp;
			while(listeBrainsTmp.size()<NBR_SELECTION){
				int max = listeBrains[0].score, index_max = 0;
				for(int i(1);i<listeBrains.size();i++){
					if(listeBrains[i].score>max){
						index_max = i;
						max = listeBrains[i].score;
					}
				}
				listeBrainsTmp.push_back(listeBrains[index_max]);
				listeBrains[index_max].score = 0;
			}
			listeBrains.clear();

			cout << "liste triée" << endl;
			for(int i(0);i<listeBrainsTmp.size();i++){
				cout << i << ", " << listeBrainsTmp[i].score << endl;
			}
			cout << "fin de liste triée" << endl;

			//on construit la nouvelle selection

			//on mets le premier sans aucun changement
			listeBrains.push_back(listeBrainsTmp[0]);
			//on en ajoute aux hasards
			for(int i(0);i<NBR_RANDOM;i++){
				VarSelection selection(listeBrains[0]);
				selection.m.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
				selection.score = 0;
				selection.best = 0;
				listeBrains.push_back(selection);
			}

			//puis on complete avec des petits babyyys
			while(listeBrains.size()<NBR_SELECTION)
			{
				//init parent
				int b = 0, a = 0;
				VarSelection parent1 = selectionRandomly(listeBrainsTmp,b), parent2 = selectionRandomly(listeBrainsTmp,a);
				//on crée une boucle qui permet d'éviter qu'un parent se croise avec lui même
				while(b==a)
					parent2 = selectionRandomly(listeBrainsTmp,a);

				cout << b << "&" << a << " --> BB" << endl;
				//init babys
				parent1.best=0;
				parent2.best=0;
				parent1.score=0;
				parent2.score=0;

				//parents become babyssss
				makeBabys(parent1.m,parent2.m);

				//ajout dans la liste
				listeBrains.push_back(parent1);
			}
			cout << "baby okayy" << endl;

			//mutation i commence à 1 pour ne pas mettre de mutation sur le premier
			for(int i(1);i<listeBrains.size();i++)
			{
				//get adn
				vector<unsigned int> adn;
				getAdn(listeBrains[i].m,adn);

				//we gonna mutate this babyyyy
				for(int j(0);j<adn.size();j++){
					if(rand()%1000+1<=FRQ_MUTATION*1000.0)
					{
						adn[j] = (1u << rand()%32) ^ adn[j];
						cout << "M";
					}
				}
				cout << " & " << endl;
				//set adn
				setAdn(listeBrains[i].m,adn);
			}

			cout << "mutation okay " << endl;

			generation++;

			//on relance avec le premier
			robotInit(robot);
			player = &(listeBrains[0]);
			robot.setBrain(&(player->m));
			if(player->score>10000){
				player->m.saveTraining("../resources/trained_model/brain.ml");
			}
		}
		//management time
		end_point = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(end_point-start_point).count();
		if(duration<1000.0/FPS&&state_space){
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(1000.0/FPS-duration)));
		}
	}

	TTF_CloseFont(police);
	TTF_CloseFont(bigFont);
    return 0;
}

void robotInit(Robot &robot){
	robot.setPos(1350.0,150.0);
	robot.setRotation(M_PI/3.0);
	robot.clearTick();
}


void evaluateRobot(Robot &robot, VarSelection *player, bool &f){
	//si trop long, on arrete
	robot.update();

	if(robot.getDuration()>TIMEOUT){
		robot.setAlive(false);
	}
	f = false;
	//si il est mort ou à gagner, on gére la sélection
	if(!robot.isAlive()||robot.getWin()){
		//si il a reussi, il doit essayer d'avoir le chemin le plus court
		if(robot.getWin()){
			player->score = int(20000.0-robot.getDistanceDone());
		//sinon le plus long
		}else{
			player->score = int(robot.getDistanceDone());
		}
		f = true;
	}
}



void makeBabys(MachineLearning &m1, MachineLearning &m2)
{
	//get the adn 
	vector<unsigned int> adn1, adn2, adnT1, adnT2;
	//pour qu'il prenne les meme réseaux de neurone
	getAdn(m1,adn1);
	getAdn(m2,adn2);
	ofstream log("../resources/logs/logBabys.txt");

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
		ofstream log("../resources/logs/errorSetADN.txt");
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
