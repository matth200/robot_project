#ifndef INCLUDE_WORLD
#define INCLUDE_WORLD

#include <fstream>
#include <iostream>

#include <vector>

#include <cmath>
#include <cstring>

#include <SDL/SDL.h>

#include "draw.h"

class World{
    public:
        World();
        ~World();
        void draw(SDL_Surface *screen);
        bool loadMap(const char* filename);
    protected:
        std::vector<Line> _carte, _carte_red, _carte_green;
};

// class VirtualWorld{
//     public:
//         VirtualWorld();
//         ~VirtualWorld();

//     protected:
//         std::vector<> *_map;
// };



#endif