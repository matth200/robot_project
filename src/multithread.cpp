#include "multithread.h"
using namespace std;

//Population
Population::Population(){
    finished = false;
}

Population::~Population(){

}

void Population::evaluatePopulation(){
    //on determine les scores de chaque brain
    //cout << "affichage non classé" << endl;

    //cout << 0 << ", score:" << player->score << endl;
    VarSelection *player;
    for(int i(1);i<_listeBrains.size();i++){
        player = &(_listeBrains[i]);
        player->score = 0;
        robot.setBrain(&(player->m));
        _universe.initStep();
        //on le fait jouer tout seul
        while(!_universe.isFinished()){
            bool finish = false;
            robotInit(robot);
            while(!finish){
                evaluateRobot(robot, player, finish);
            }
            _universe.nextStep();
        }
        cout << i << ", score:" << player->score << endl;
    }
    //_universe.initStep();
    
}


//ManagerThread
ManagerThread::ManagerThread(int nbr_thread, int *max_score):_nbr_thread(nbr_thread), _max_score(max_score)
{

}


void ManagerThread::createPopulation(){

}

vector<VarSelection> ManagerThread::getNewGeneration(){
    vector<VarSelection> _listeBrains;
    
    //on recupere la liste de chaque thread



    //on les classe par rapport à leur score
    vector<VarSelection> _listeBrainsTmp;
    while(_listeBrainsTmp.size()<NBR_SELECTION){
        int max = _listeBrains[0].score, index_max = 0;
        for(int i(1);i<_listeBrains.size();i++){
            if(_listeBrains[i].score>max){
                index_max = i;
                max = _listeBrains[i].score;
            }
        }
        _listeBrainsTmp.push_back(_listeBrains[index_max]);
        _listeBrains[index_max].score = 0;
    }
    _listeBrains.clear();

    //cout << "liste triée" << endl;
    for(int i(0);i<_listeBrainsTmp.size();i++){
        cout << i << ", " << _listeBrainsTmp[i].score << endl;
    }
    //cout << "fin de liste triée" << endl;

    //on construit la nouvelle selection

    //on mets le premier sans aucun changement
    _listeBrains.push_back(_listeBrainsTmp[0]);
    *_max_score = _listeBrains[0].score;
    _listeBrains[0].score = 0;

    //puis on complete avec des petits babyyys
    while(_listeBrains.size()<NBR_POPULATION-NBR_RANDOM)
    {
        //init parent
        int b = 0, a = 0;
        VarSelection parent1 = selectionRandomly(_listeBrainsTmp,b), parent2 = selectionRandomly(_listeBrainsTmp,a);
        //on crée une boucle qui permet d'éviter qu'un parent se croise avec lui même
        while(b==a)
            parent2 = selectionRandomly(_listeBrainsTmp,a);

        //cout << b << "&" << a << " --> BB" << endl;
        //init babys
        parent1.best=0;
        parent2.best=0;
        parent1.score=0;
        parent2.score=0;

        //parents become babyssss
        makeBabys(parent1.m,parent2.m);

        //ajout dans la liste
        _listeBrains.push_back(parent1);
    }
    //cout << "baby okayy" << endl;

    //mutation i commence à 1 pour ne pas mettre de mutation sur le premier
    for(int i(1);i<_listeBrains.size();i++)
    {
        //get adn
        vector<unsigned int> adn;
        getAdn(_listeBrains[i].m,adn);

        //we gonna mutate this babyyyy
        for(int j(0);j<adn.size();j++){
            if(rand()%1000+1<=FRQ_MUTATION*1000.0)
            {
                adn[j] = (1u << rand()%32) ^ adn[j];
                //cout << "M";
            }
        }
        //cout << " & " << endl;
        //set adn
        setAdn(_listeBrains[i].m,adn);
    }

    //cout << "mutation okay " << endl;

    //on en ajoute aux hasards
    for(int i(0);i<NBR_RANDOM;i++){
        VarSelection selection(_listeBrains[0]);
        selection.m.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
        selection.score = 0;
        selection.best = 0;
        _listeBrains.push_back(selection);
    }
    //cout << "ajout aux hasards okayy" << endl;

    return _listeBrains;
}

bool ManagerThread::isFinished(){
    for(int i(0);i<_listePopulation.size();i++){
        if(!_listePopulation[i].isFinished()){
            return false;
        }
    }
    return true;
}

void ManagerThread::clear(){
    _listePopulation.clear();
}