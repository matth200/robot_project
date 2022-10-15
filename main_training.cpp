//#define NO_GUI
#define TRY_BEST true

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

//multithread
#include "src/multithread.h"
#define NBR_THREAD 25

#include "src/utils.h"
#include "src/genetic_algorithm_const.h"

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
#define MAX_TIME_ESC 500
#define FPS 40.0

//parametre GENETIC_ALGORITHM
#include "src/genetic_algorithm_const.h"

//utils
#include "src/utils.h"


//variable pour effectuer la selection
#define SAVE_NAME "brain_2_20_"


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
	string bestfilename = "";
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
				bestfilename = name;
			}
		}
	}
	cout << "Filename:" << bestfilename <<  ", max_score:" << max_score_folder << endl;


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
		//m->addColumn(20);
		m->addColumn(4);

		selection.best = false;
		selection.score = 0;

		m->setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
		listeBrains.push_back(selection);
	}


	//configuration ou non du premier machine learning
	bool state_backup_data = false;
	if(argc<=2||TRY_BEST){
		//si un fichier est proposé
		if(argc==2||TRY_BEST){
			cout << "Insertion du réseau de neurones déjà entrainé...." << endl;
			const char *filename = (TRY_BEST)?bestfilename.c_str():argv[1];
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
		display.setBackup(string((TRY_BEST)?bestfilename:argv[1]));
	}
	#endif

	//multithread
	//on enleve un thread car on a déjà le principal ici dans main()
	ManagerThread managerThread(NBR_THREAD, &max_score);


	//speed
	bool state_space = true, state_enter = false;
	bool behind_work = false;
	bool state_save = false;

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
							robot.setMotor2(ROBOT_SPEED);
							break;
						case SDLK_LEFT:
							robot.setMotor1(ROBOT_SPEED);
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
							robot.setMotor2(0);
							break;
						case SDLK_LEFT:
							robot.setMotor1(0);
							break;
					}
					break;
			}
		}
		//cout << "On entre dans la boucle !" << endl;
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
				#ifndef NO_GUI
				this_thread::sleep_for(chrono::milliseconds(200));
				#endif
			}
			//sinon on gere tous les autres en arriere plan
			if(universe.isFinished()){
				state_save = true;
				//mettre le manager ici
				ManagerThread manager(NBR_THREAD,&max_score);
				manager.createPopulation(&listeBrains, universe, robot);

				while(!manager.isFinished()){
					manager.displayStat();
					this_thread::sleep_for(chrono::milliseconds(100));
				}

				listeBrains.clear();
				listeBrains = manager.getNewGeneration();
				cout << listeBrains.size() << endl;

				//on prépare pour la prochaine génération
				player = &(listeBrains[0]);
				universe.initStep();
				robotInit(robot);
				robot.setBrain(&player->m);
				generation++;

				#ifndef NO_GUI
				display.setNeuralNetwork(&(player->m));
				display.setRobot(&robot);
				display.setUniverse(&universe);
				#endif
			
				cout << "Generation:" << generation << ", score_max:" << max_score << ", max_score_folder:" << max_score_folder << endl << endl;
				file_evolution << generation << " " << max_score << endl;
			}
		}

		//pour éviter de perdre les bons éléments à chaque génération
		if(max_score>max_score_folder-100000&&state_save){
			string filename_maxscore = (string("../resources/trained_model/")+SAVE_NAME+to_string(max_score)+".ml");
			player->m.saveTraining(filename_maxscore.c_str());
			cout << "Enregistrement sur " << filename_maxscore << endl;
			if(max_score>max_score_folder){
				max_score_folder = max_score;
			}
			state_save = false;
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
		player->m.saveTraining((string("../resources/trained_model/")+SAVE_NAME+to_string(max_score)+".ml").c_str());
	}
    return 0;
}