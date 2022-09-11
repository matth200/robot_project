#ifndef INCLUDE_WORLD
#define INCLUDE_WORLD

#include <fstream>
#include <iostream>

#include <vector>

#include <cmath>
#include <cstring>

#include <SDL/SDL.h>

#include "draw.h"


struct Element{
    Line *p_line;
    int state;
};


class VirtualWorld{
    public:
        VirtualWorld();
        ~VirtualWorld();
        void setSize(int w, int h);
    protected:
        std::vector<std::vector<Element>> *_map;
};

class World{
    public:
        World();
        ~World();
        VirtualWorld* getRealWorld();
        void draw(SDL_Surface *screen);
        bool loadMap(const char* filename);
    protected:
        std::vector<Line> _carte, _carte_red, _carte_green;
        VirtualWorld real_map;
};



#endif