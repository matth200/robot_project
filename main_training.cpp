#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <cmath>

using namespace std;

#define RANDOM_VALUE_W 10
#define RANDOM_VALUE_B 10

void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m);

int main(int argc, char **argv){



    return 0;
}

void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m)
{
	int size = 24;
	for(int j(0);j<m.getNumberColumn();j++)
	{
		for(int i(0);i<m.getNetwork(j)->get_number_neuron();i++)
		{
			//on affiche chaque neurone avec sa valeur plus ou moins blanche
			double value = m.getNetwork(j)->get_neuron(i)->get_value();
			Uint32 color = SDL_MapRGB(screen->format,value*255.0,50+value*205.0,value*255.0);

			//si c'est la direction souhaité
			if(m.getNumberColumn()-1==j&&i==m.getPrediction())
				color = SDL_MapRGB(screen->format,25,200,200);

			drawSquare(screen,SCREEN_WIDTH+40+j*100,50+((size-m.getNetwork(j)->get_number_neuron())/2.0+i)*30,20,20,color);
			for(int a(0);a<m.getNetwork(j)->get_neuron(i)->numberConnection()&&j!=0;a++)
			{
				value = m.getNetwork(j-1)->get_neuron(a)->get_value();
				color = SDL_MapRGB(screen->format,value*25,value*25,value*200);
				if(m.getNetwork(j)->get_neuron(i)->get_weight(a)<0)
					color = SDL_MapRGB(screen->format,value*200,value*25,value*25);
				drawLine(screen,SCREEN_WIDTH+40+(j-1)*100+20,50+((size-m.getNetwork(j-1)->get_number_neuron())/2.0+a)*30+10,SCREEN_WIDTH+40+j*100,50+((size-m.getNetwork(j)->get_number_neuron())/2.0+i)*30+10,color);
			}
		}
	}
}