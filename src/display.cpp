#include "display.h"
using namespace std;


Display::Display(int x, int y, int width, int height):_x(x),_y(y),_width(width),_height(height){
    _machine = NULL;
    _police = NULL;
    _texte_surface = NULL;
    _robot = NULL;
}

Display::~Display(){
    SDL_FreeSurface(_texte_surface);
}

void Display::setFont(TTF_Font *police){
    _police = police;
}

void Display::setNeuralNetwork(MachineLearning *machine){
    _machine = machine;
}

void Display::draw(SDL_Surface *screen){
	//barre qui sépare le score du jeu
	drawSquare(screen,_x,0,3,_height,COLOR_WHITE);
	drawNeuralNetwork(screen, *_machine, _x+60, 100);

    if(!_robot->isAlive()){
        if(_texte_surface!=NULL){
            SDL_FreeSurface(_texte_surface);
        }
        if(!_robot->getWin()){
            _texte_surface = TTF_RenderText_Solid(_police, "DEAD", SDL_Color({255,255,255}));
        }else{
            _texte_surface = TTF_RenderText_Solid(_police, "WIN", SDL_Color({255,255,255}));
        }
        _pos.x = 550;
        _pos.y = 400;
        SDL_BlitSurface(_texte_surface, NULL, screen, &_pos);
    }

}
void Display::setRobot(Robot *robot){
    _robot = robot;
}

void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m, int x, int y)
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

			drawSquare(screen,x+j*100,y+((size-m.getNetwork(j)->get_number_neuron())/2.0+i)*30,20,20,color);
			for(int a(0);a<m.getNetwork(j)->get_neuron(i)->numberConnection()&&j!=0;a++)
			{
				value = m.getNetwork(j-1)->get_neuron(a)->get_value();
				color = SDL_MapRGB(screen->format,value*25,value*25,value*200);
				if(m.getNetwork(j)->get_neuron(i)->get_weight(a)<0)
					color = SDL_MapRGB(screen->format,value*200,value*25,value*25);
				//color = SDL_MapRGB(screen->format,200,25,25);
				drawLine(screen,x+(j-1)*100+20,y+((size-m.getNetwork(j-1)->get_number_neuron())/2.0+a)*30+10,x+j*100,y+((size-m.getNetwork(j)->get_number_neuron())/2.0+i)*30+10,color);
			}
		}
	}
}