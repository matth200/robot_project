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

#define WORLD_WHITE 0
#define WORLD_RED 1
#define WORLD_GREEN 2

class VirtualWorld{
    public:
        VirtualWorld();
        ~VirtualWorld();
        void setSize(int w, int h);
        void addLine(Line &line, int state);
        std::vector<Element> getDetection(Line &line, int &distance);
    protected:
        std::vector<std::vector<std::vector<Element>>> *_map;
};

class World{
    public:
        World(int w, int h);
        ~World();
        VirtualWorld* getRealWorld();
        void buildVirtualWorld();
        void draw(SDL_Surface *screen);
        bool loadMap(const char* filename);
    protected:
        std::vector<Line> _carte, _carte_red, _carte_green;
        VirtualWorld real_map;
};



#endif