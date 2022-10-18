#include "utils.h"
using namespace std;


void robotInit(Robot &robot){
	//robot.setPos(1350.0,150.0+rand()%700);

	//on place aux points décider dans l'univers
	robot.setUniversePos();

	//orientation aux hasards
	double angle = (rand()%360)/180.0*M_PI;
	robot.setRotation(angle);
	robot.setNoiseStart(angle);
	robot.clearTick();
}


int evaluateRobot(Robot &robot, VarSelection *player, bool &f){
	//si trop long, on arrete
	robot.update();
	int score = (player!=NULL)?player->score:0;

	if(robot.getDuration()>TIMEOUT){
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
	if(player!=NULL){
		player->score = score;
	}
	return score;
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
