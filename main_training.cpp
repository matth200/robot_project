//#define NO_GUI

#include <fstream>
#include <iostream>
#include <cmath>

//random librairy
#include <cstdlib>
#include <ctime>

//management time
#include <chrono>
#include <thread>

#ifndef NO_GUI
//SDL
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#endif

#include <vector>

//machine learning
#include "src/m_learning.h"

//world
#include "src/world.h"

//draw
#ifndef NO_GUI
#include "src/draw.h"
#endif


//car
#include "src/car.h"
#ifndef NO_GUI
//affichage information
#include "src/display.h"
#endif

//include fs
#include <filesystem>
#include <regex>
namespace fs = std::filesystem;
#define TRAINMODEL_FOLDER "../resources/trained_model/"

//include time
#include <ctime>
#define STATS_FOLDER "../resources/stats/"

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
#define NBR_SELECTION 100
#define NBR_RANDOM 30
#define TIMEOUT 60000

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
#define SAVE_NAME "brain_2_20_"

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

	#ifndef NO_GUI
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

	SDL_WM_SetCaption("NEURAL NETWORK TRAINING", NULL);
	#endif

	//on récupére le meilleur score
	string filename = "";
	string path = TRAINMODEL_FOLDER;
	cout << "Selection du meilleur réseau de neurones dans "<< path << endl;
	int max_score_folder = 0;
	smatch m;
	regex r(string(SAVE_NAME)+"([0-9]+).ml");
	for (const auto & entry : fs::directory_iterator(path)){
		string name = entry.path();
		regex_search(name,m,r);
		int score = 0;
		if(m.size()>0){
			auto v = m[1];
			score = stoi(v);
			if(score>max_score_folder){
				max_score_folder = score;
				filename = name;
			}
		}
	}
	cout << "Filename:" << filename <<  ", max_score:" << max_score_folder << endl;


	//on prepare le fichier de suivi de l'evolution
	time_t now_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
	string filename_evolution = string(STATS_FOLDER)+"evolution_"+ctime(&now_time);
	for(int i(0);i<filename_evolution.length();i++){
		if(filename_evolution[i]==' '){
			filename_evolution[i] = '_';
		}
	}
	cout << "filename_evolution:" << filename_evolution << endl;
	ofstream file_evolution(filename_evolution);

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


	//on prepare le genetic algorithm
	vector<VarSelection> listeBrains;
	//on commence avec toutes la selection aux hasards
	for(int i(0);i<NBR_POPULATION;i++){
		VarSelection selection;
		MachineLearning *m = &(selection.m);
		m->open(4);
		m->addColumn(20);
		m->addColumn(20);
		m->addColumn(4);

		selection.best = false;
		selection.score = 0;

		m->setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
		listeBrains.push_back(selection);
	}


	//configuration ou non du premier machine learning
	bool state_backup_data = false;
	if(argc<=2){
		//si un fichier est proposé
		if(argc==2){
			cout << "Insertion du réseau de neurones déjà entrainé...." << endl;
			const char *filename = argv[1];
			if(listeBrains[0].m.backupTraining(filename)){
				cout << "Récupération du réseau de neurones effectuée avec succèes" << endl;
				state_backup_data = true;
			}else{
				cout << "Erreur lors de la récupération" << endl;
				cout << "Retour au mode NORMAL(Valeurs aléatoire)" << endl;
				listeBrains[0].m.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
			}
		}
	}else{
		cout << "Mauvaise utilisation des parmètres" << endl;
		cout << argv[0] << " 'chemin_dacces_fichier_ml'" << endl;
	}

	//Machine learning, on fait jouer le joueur 1
	VarSelection *player = &(listeBrains[0]);
	int generation = 0;
	int max_score = 0;
	
	//Robot
	Robot robot;
	robot.setBrain(&(player->m));
	robot.connectToUniverse(&universe);
	robotInit(robot);

	#ifndef NO_GUI
	//Display information
	Display display(SCREEN_WIDTH, 0, OUTSCREEN_W, SCREEN_HEIGHT);
	display.setNeuralNetwork(&(player->m));
	display.setRobot(&robot);
	display.setUniverse(&universe);


	//on place le nom du fichier récupéré
	if(state_backup_data){
		display.setBackup(string(argv[1]));
	}
	#endif


	//speed
	bool state_space = false, state_enter = false;
	bool behind_work = false;

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
						case 13:
							if(state_enter==false){
								player->m.saveTraining((string("../resources/trained_model/")+SAVE_NAME+to_string(max_score)+".ml").c_str());
								state_enter = true;
							}
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
		#ifndef NO_GUI
		SDL_FillRect(screen, NULL, COLOR_BLACK);
		#endif

		bool finish = false;
		if(!behind_work){
			//viens faire avancer le robot et remplir si terminé le score du player
			evaluateRobot(robot, player, finish);

			//on récupère le meilleur score en live aussi
			if(max_score<player->score){
				max_score = player->score;
			}
		}

		#ifndef NO_GUI
		//on gère les infos
		display.setInfo(generation, max_score);
		if(state_enter==true){
			display.setSave(true);
		}

		if(!behind_work){
			//affichage de l'environement
			universe.getCurrentWorld()->draw(screen);
			robot.draw(screen);
		}
		display.draw(screen);


		//on affiche
		SDL_Flip(screen);
		#endif

		if(finish){
			//si pas fini on passe au niveau d'au dessus
			universe.nextStep();
			if(!universe.isFinished()){
				robotInit(robot);
				this_thread::sleep_for(chrono::milliseconds(200));
			}
			//sinon on gere tous les autres en arriere plan
			if(universe.isFinished()){
				//on determine les scores de chaque brain
				cout << "affichage non classé" << endl;

				cout << 0 << ", score:" << player->score << endl;
				for(int i(1);i<listeBrains.size();i++){
					player = &(listeBrains[i]);
					player->score = 0;
					robot.setBrain(&(player->m));
					universe.initStep();
					//on le fait jouer tout seul
					while(!universe.isFinished()){
						finish = false;
						robotInit(robot);
						while(!finish){
							evaluateRobot(robot, player, finish);
						}
						universe.nextStep();
					}
					cout << i << ", score:" << player->score << endl;
				}
				universe.initStep();
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
				max_score = listeBrains[0].score;
				listeBrains[0].score = 0;

				//puis on complete avec des petits babyyys
				while(listeBrains.size()<NBR_POPULATION-NBR_RANDOM)
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

				//on en ajoute aux hasards
				for(int i(0);i<NBR_RANDOM;i++){
					VarSelection selection(listeBrains[0]);
					selection.m.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
					selection.score = 0;
					selection.best = 0;
					listeBrains.push_back(selection);
				}
				cout << "ajout aux hasards okayy" << endl;

				//cout << "nombree" << listeBrains.size() << endl;

				//on fait suivre l'évolution
				file_evolution << generation << " " << max_score << endl;

				//on passe à la génération d'après
				generation++;

				//on relance avec le premier
				robotInit(robot);
				player = &(listeBrains[0]);
				robot.setBrain(&(player->m));
				cout << "Generation:" << generation << ", max_score:" << max_score << endl;
			}
		}

		//pour éviter de perdre les bons éléments à chaque génération
		if(max_score_folder!=0&&max_score>=max_score_folder){
			player->m.saveTraining((string("../resources/trained_model/")+SAVE_NAME+to_string(player->score)+".ml").c_str());
		}

		#ifndef NO_GUI
		//management time
		end_point = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(end_point-start_point).count();
		if(duration<1000.0/FPS&&state_space){
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(1000.0/FPS-duration)));
		}
		#endif
	}

	//sécurité pour ne pas perdre les bons entrainements
	if(max_score>=600000){
		player->m.saveTraining((string("../resources/trained_model/")+SAVE_NAME+to_string(player->score)+".ml").c_str());
	}
    return 0;
}

void robotInit(Robot &robot){
	//robot.setPos(1350.0,150.0+rand()%700);

	//on place aux points décider dans l'univers
	robot.setUniversePos();

	//orientation aux hasards
	robot.setRotation((rand()%360)/180.0*M_PI);
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
			player->score += int(50000.0-robot.getDistanceDone());
		//sinon le plus long
		}else{
			player->score += int(robot.getDistanceDone());
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
