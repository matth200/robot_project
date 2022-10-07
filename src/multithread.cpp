#include "multithread.h"
using namespace std;

//Population
Population::Population(vector<VarSelection> *listeBase, int cursor, int size, Universe &originalUniverse, Robot &originalRobot){
    _finished = false;
    _copyUniverse = new Universe(originalUniverse);
    _copyRobot = new Robot(originalRobot);

    clear();


    //on ajoute les éléments à prendre en compte
    for(int i(0);i<size;i++){
        _listeBrain.push_back((*listeBase)[i+cursor]);
    }
}

Population::~Population(){
    delete _copyUniverse;
    delete _copyRobot;
}

void Population::evaluate(){
    //on determine les scores de chaque brain
    //cout << "affichage non classé" << endl;

    //cout << 0 << ", score:" << player->score << endl;
    VarSelection *player;
    _mutex_liste.lock();
    int size = _listeBrain.size();
    _mutex_liste.unlock();
    for(int i(0);i<size;i++){
        _mutex_liste.lock();
        player = &(_listeBrain[i]);
        _mutex_liste.unlock();

        player->score = 0;
        _copyRobot->setBrain(&(player->m));
        _copyUniverse->initStep();

        //on le fait jouer tout seul
        while(!_copyUniverse->isFinished()){
            bool finish = false;
            robotInit(*_copyRobot);
            while(!finish){
                evaluateRobot(*_copyRobot, player, finish);
            }
            _copyUniverse->nextStep();
        }
        //cout << i << ", score:" << player->score << endl;
        _mutex_cursor.lock();
        _cursor = i+1;
        _mutex_cursor.unlock();
    }
    _copyUniverse->initStep();
    _mutex_finish.lock();
    _finished = true;
    _mutex_finish.unlock();
}

int Population::getSize(){
    int size;
    _mutex_liste.lock();
    size = _listeBrain.size();
    _mutex_liste.unlock();
    return size;
}

int Population::getCursor(){
    int cursor;
    _mutex_cursor.lock();
    cursor = _cursor;
    _mutex_cursor.unlock();
    return cursor;
}

vector<VarSelection>* Population::getListe(){
    return &_listeBrain;
}

bool Population::isFinished(){
    bool finished;
    _mutex_finish.lock();
    finished = _finished;
    _mutex_finish.unlock();
    return finished;
}

void Population::clear(){
    _listeBrain.clear();
    _cursor = 0;
    _finished=false;
    //on initialise l'univers et le robot
    _copyUniverse->initStep();
	_copyRobot->connectToUniverse(_copyUniverse);
	robotInit(*_copyRobot);
}


//ManagerThread
ManagerThread::ManagerThread(int nbr_thread, int *max_score):_nbr_thread(nbr_thread), _max_score(max_score)
{
    _size = 0;
    loadbar = NULL;
}

ManagerThread::~ManagerThread(){

    for(int i(0);i<_nbr_thread;i++){
        //delete _listePopulation[i];
        //delete _threads[i];
    }

    delete loadbar;
}

void ManagerThread::createPopulation(vector<VarSelection> *listeBase, Universe &originalUniverse, Robot &originalRobot){

    //on repartie convenablement la population
    int size = listeBase->size();
    _size = size;

    delete loadbar;
    loadbar = new LoadBar(_size);

    vector<int> repartition(_nbr_thread,size/_nbr_thread);
    int missOne = size - size/_nbr_thread * _nbr_thread;
    for(int i(0);i<missOne;i++){
        repartition[i%_nbr_thread] += 1;
    }

    clear();

    //on crée chaque population et thread
    int sum = 0;
    for(int i(0);i<_nbr_thread;i++){
        //on crée la population
        _listePopulation.push_back(new Population(listeBase, sum, repartition[i], originalUniverse, originalRobot));
        sum+=repartition[i];

        //on lance le thread
        _threads.push_back(new thread(&Population::evaluate, _listePopulation[i]));
    }

    //pour vérifier le compte
    //cout << "On a sum:" << sum << " et size:" << size << endl;
}

vector<VarSelection> ManagerThread::getNewGeneration(){
    vector<VarSelection> _listeBrains;
    
    //on recupere la liste de chaque thread
    vector<VarSelection>* tmpListe = NULL;
    for(int i(0);i<_listePopulation.size();i++){
        tmpListe = _listePopulation[i]->getListe();
        for(int j(0);j<tmpListe->size();j++){
            _listeBrains.push_back((*tmpListe)[j]);
        }
    }

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

    cout << "liste triée" << endl;
    for(int i(0);i<_listeBrainsTmp.size();i++){
        cout << i << ", " << _listeBrainsTmp[i].score << endl;
    }
    cout << "fin de liste triée" << endl;

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
    cout << "baby okayy" << endl;

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
                cout << "M";
            }
        }
        cout << " & " << endl;
        //set adn
        setAdn(_listeBrains[i].m,adn);
    }

    cout << "mutation okay " << endl;

    //on en ajoute aux hasards
    for(int i(0);i<NBR_RANDOM;i++){
        VarSelection selection(_listeBrains[0]);
        selection.m.setWeightRandom(RANDOM_VALUE_W,RANDOM_VALUE_B);
        selection.score = 0;
        selection.best = 0;
        _listeBrains.push_back(selection);
    }
    cout << "ajout aux hasards okayy" << endl;

    return _listeBrains;
}

bool ManagerThread::isFinished(){
    for(int i(0);i<_listePopulation.size();i++){
        if(!_listePopulation[i]->isFinished()){
            return false;
        }
    }
    cout << "on attend les threads" << endl;
    //on attend la fin des threads
    for(int i(0);i<_nbr_thread;i++){
        _threads[i]->join();
    }
    cout << "fin de l'attente" << endl;
    return true;
}

int ManagerThread::getAvancement(){
    int sum = 0;
    for(int i(0);i<_listePopulation.size();i++){
        sum += _listePopulation[i]->getCursor();
    }
    return sum-1;
}

void ManagerThread::displayStat(){
    loadbar->show(getAvancement());
}

void ManagerThread::clear(){
    _listePopulation.clear();
}