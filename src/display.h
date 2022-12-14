#ifndef INCLUDE_DISPLAY
#define INCLUDE_DISPLAY

#include "m_learning.h"
#include "draw.h"
#include "car.h"
#include "world.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m, int x, int y);

class Display{
    public:
        Display(int x, int y, int width, int height);
        ~Display();
        void setNeuralNetwork(MachineLearning *machine);
        void setFont(TTF_Font *police);
        void setInfo(int generation, int max_score, int score);
        void setBigFont(TTF_Font *police);
        void setMiniFont(TTF_Font *police);
        void draw(SDL_Surface *screen);
        void setRobot(Robot *robot);
        void setUniverse(Universe *universe);
        void setSave(bool state);
        void setBackup(std::string filename);
    protected:
        MachineLearning *_machine;
        int _width, _height, _x, _y;
        TTF_Font *_police, *_big_police, *_police_mini;
        SDL_Rect _pos;
        Robot *_robot;
        SDL_Surface *_texte_surface;
        int _generation, _max_score;
        Universe *_universe;
        bool _save;
        std::string _filename;
        int _score;
};




#endif