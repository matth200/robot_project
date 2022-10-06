#ifndef INCLUDE_MULTITHREAD
#define INCLUDE_MULTITHREAD

#include <thread>
#include <mutex>
#include <vector>
#include "genetic_algorithm_const.h"
#include "utils.h"


class Population{
    public:
        Population();
        ~Population();
        void evaluate();
        bool isFinished();
    protected:
        bool finished;
        vector<VarSelection> _listeBrain;
};

class ManagerThread{
    public:
        ManagerThread(int nbr_thread, int *max_score);
        ~ManagerThread();
        void createPopulation();
        vector<VarSelection> getNewGeneration();
        bool isFinished();
        void clear();
    protected:
        int _nbr_thread;
        vector<thread*> _threads;
        vector<Population> _listePopulation;
        int *_max_score;
        
};

#endif