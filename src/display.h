#ifndef INCLUDE_DISPLAY
#define INCLUDE_DISPLAY

#include "m_learning.h"
#include "draw.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m, int x, int y);

class Display{
    public:
        Display(int x, int y, int width, int height);
        ~Display();
        void setNeuralNetwork(MachineLearning *machine);
        void setFont(TTF_Font *police);
        void draw(SDL_Surface *screen);
    protected:
        MachineLearning *_machine;
        int _width, _height, _x, _y;
        TTF_Font *_police;
};




#endif