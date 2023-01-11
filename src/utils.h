#ifndef UTILS_H
#define UTILS_H

#include "m_learning.h"
#include "car.h"
#include "genetic_algorithm_const.h"
#include <vector>
#include <cmath>


struct VarSelection
{
	MachineLearning m;
	int score;
	bool best;
};

//genetic algorithm
void getAdn(MachineLearning &m, std::vector<unsigned int> &adn);
void setAdn(MachineLearning &m, std::vector<unsigned int> &adn);
void makeBabys(MachineLearning &m1, MachineLearning &m2);
VarSelection selectionRandomly(std::vector<VarSelection> &players, int &a);
double distance(int x1, int y1, int x2, int y2);

int evaluateRobot(Robot &r, VarSelection *player, bool &f);
void robotInit(Robot &robot);

#endif