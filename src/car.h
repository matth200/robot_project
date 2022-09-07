#ifndef INCLUDE_CAR
#define INCLUDE_CAR

#include <SDL/SDL.h>

class Car{
    public:
        Car();
        ~Car();
        void draw(SDL_Surface *screen);
    protected:
        int x,y;
};

#endif