#ifndef INCLUDE_MULTITHREAD
#define INCLUDE_MULTITHREAD

#include <thread>
#include <mutex>
#include <vector>
#include "genetic_algorithm_const.h"
#include "utils.h"
#include "liveTerminal.h"


class Population{
    public:
        Population(std::vector<VarSelection> *listeBase, int cursor, int size, Universe &originalUniverse, Robot &originalRobot);
        ~Population();
        void evaluate();
        int getSize();
        int getCursor();
        std::vector<VarSelection>* getListe();
        bool isFinished();
        void clear();
    protected:
        bool _finished;
        std::mutex _mutex_finish;
        std::vector<VarSelection> _listeBrain;
        std::mutex _mutex_liste;
        Universe *_copyUniverse;
        Robot *_copyRobot;
        int _cursor;
        std::mutex _mutex_cursor;

};

class ManagerThread{
    public:
        ManagerThread(int nbr_thread, int *max_score);
        ~ManagerThread();
        void createPopulation(std::vector<VarSelection> *listeBase, Universe &originalUniverse, Robot &originalRobot);
        std::vector<VarSelection> getNewGeneration();
        bool isFinished();
        void clear();
        void displayStat();
        int getAvancement();
    protected:
        int _nbr_thread;
        std::vector<std::thread*> _threads;
        std::vector<Population*> _listePopulation;
        int *_max_score;
        int _size;
        LoadBar *loadbar;
};

#endif